#include <vector>
#include "../WinStd.h"
#include "PipelineState.h"
#include "CommandQueueManager.h"
#include "CommandListManager.h"

namespace stay
{
	const size_t CommandQueue::sc_NumMaxExcuteCommandList = 10;

	CommandQueue::CommandQueue() :
		m_type(D3D12_COMMAND_LIST_TYPE_DIRECT),
		m_commandQueue(nullptr),
		m_fence(nullptr),
		m_fenceEvent(nullptr),
		m_lastCompleteFenceValue(0),
		m_nextFenceValue(1)
	{

	}

	CommandQueue::~CommandQueue()
	{
		ShutDown();
	}

	bool CommandQueue::IsComplete(UINT64 fenceValue)
	{
		UINT64 currentCompletefFenceValue = m_fence->GetCompletedValue();

		m_lastCompleteFenceValue = std::max(currentCompletefFenceValue, m_lastCompleteFenceValue);
		return fenceValue <= currentCompletefFenceValue;
	}

	void CommandQueue::WaitForFence(UINT64 fenceValue)
	{
		if (IsComplete(fenceValue))
		{
			return;
		}

		m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, false);
	}

	void CommandQueue::WaitLastComplate()
	{
		WaitForFence(m_nextFenceValue - 1);
	}

	ID3D12CommandAllocator* CommandQueue::RequierCommandListAllocator()
	{

		return m_allocatorPool.AllocateCommandAllocator(m_fence->GetCompletedValue());
	}

	void CommandQueue::DiscardCommandListAllocator(UINT64 fenceValue, ID3D12CommandAllocator* allocator)
	{
		m_allocatorPool.DeallocateCommandAllocator(fenceValue, allocator);
	}

	void CommandQueue::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
	{
		ASSERT(device != nullptr);
		m_type = type;
		m_device = device;
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = m_type;
		queueDesc.NodeMask = 0;
		THROW_IF_FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

		m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		ASSERT(m_fenceEvent != NULL);

		m_allocatorPool.Create(device, type);
	}







	void CommandQueue::ShutDown()
	{
		if (m_fenceEvent != nullptr && CloseHandle(m_fenceEvent) == 0)
		{
			THROW_LASTEXCEPTION();
		}
		SAFE_RELEASE(m_commandQueue);
	}

	UINT64 CommandQueue::ExecuteCommandList(ID3D12CommandList* const commandList)
	{
		return ExecuteCommandLists(1u, &commandList);
	}

	UINT64 CommandQueue::ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* const* commandLists)
	{
		ID3D12CommandList* lists[sc_NumMaxExcuteCommandList] = {};

		for (size_t i = 0; i < numCommandLists; i++)
		{
			lists[i] = commandLists[i];
		}

		m_commandQueue->ExecuteCommandLists(numCommandLists, lists);

		m_commandQueue->Signal(m_fence, m_nextFenceValue);

		return m_nextFenceValue++;
	}



	void CommandQueueManager::Initialize(ID3D12Device* device)
	{
		ASSERT(device != nullptr);
		m_device = device;
		m_graphicsQueue.Create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	ID3D12CommandAllocator* CommandQueueManager::GetAllocator(const D3D12_COMMAND_LIST_TYPE commandListType)
	{
		ID3D12CommandAllocator* allocator = nullptr;
		switch (commandListType)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		case D3D12_COMMAND_LIST_TYPE_BUNDLE: allocator = m_graphicsQueue.RequierCommandListAllocator();

		}

		return allocator;
	}

	void  CommandQueueManager::CreateCommandList(const D3D12_COMMAND_LIST_TYPE commandListType, ID3D12GraphicsCommandList** commandList, ID3D12CommandAllocator** allocator, GraphicsPSO* pso, UINT nodeMask)
	{
		*allocator = GetAllocator(commandListType);
		THROW_IF_FAILED((*allocator)->Reset());
		ASSERT(allocator != nullptr);

		if (pso != nullptr)
		{
			m_device->CreateCommandList(nodeMask, commandListType, *allocator, pso->GetPipelineState(), IID_PPV_ARGS(commandList));
		}
		else
		{
			m_device->CreateCommandList(nodeMask, commandListType, *allocator, nullptr, IID_PPV_ARGS(commandList));
		}

	}

	CommandQueue& CommandQueueManager::GetQueue(D3D12_COMMAND_LIST_TYPE commandQueueType)
	{
		switch (commandQueueType)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return m_graphicsQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return m_computeQueue;
			break;
		default:
			return m_graphicsQueue;
			break;
		}
	}

	void CommandQueueManager::WaitForFence(const D3D12_COMMAND_LIST_TYPE commandListType, UINT64 fenceValue)
	{
		ASSERT(commandListType != D3D12_COMMAND_LIST_TYPE_BUNDLE);
		GetQueue(commandListType).WaitForFence(fenceValue);
	}


	void CommandQueueManager::Finalize()
	{
		m_graphicsQueue.ShutDown();
	}

}
