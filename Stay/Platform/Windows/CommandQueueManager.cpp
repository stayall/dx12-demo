#include <vector>
#include "WinStd.h"
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
		WaitForFence(--m_nextFenceValue);
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



	void CommandQueue::CreateCommandList(const D3D12_COMMAND_LIST_TYPE commandListType, CommandList* commandList)
	{
		ASSERT(commandList != nullptr && (commandListType == m_type || commandListType == D3D12_COMMAND_LIST_TYPE_BUNDLE));
		auto allcator = m_allocatorPool.AllocateCommandAllocator(m_fence->GetCompletedValue());
		commandList->Create(m_device, commandListType, allcator);

	}



	void CommandQueue::ShutDown()
	{
		m_commandQueue->Release();
		CloseHandle(m_fenceEvent);

	}

	void CommandQueue::ExecuteCommandList(ID3D12CommandList* commandList)
	{
		ExecuteCommandLists(1u, commandList);
	}

	void CommandQueue::ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* commandLists)
	{
		for (size_t i = 0; i < numCommandLists; i++)
		{
			ASSERT(commandLists->GetType() == m_type && SUCCEEDED(commandLists[i].));
			lists[i] = commandLists[i].m_commandList;
		}

		m_commandQueue->ExecuteCommandLists(numCommandLists, lists);

		m_commandQueue->Signal(m_fence, m_nextFenceValue);

		m_nextFenceValue++;
	}



	void CommandQueueManager::Initialize(ID3D12Device* device)
	{
		m_graphicsQueue.Create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}



	void CommandQueueManager::Finalize()
	{
		m_graphicsQueue.ShutDown();
	}

}
