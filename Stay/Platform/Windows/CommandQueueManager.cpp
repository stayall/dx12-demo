#include "CommandQueueManager.h"

namespace stay
{
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
		m_type(type),
		m_commandQueue(nullptr),
		m_fence(nullptr),
		m_lastCompleteFenceValue(0),
		m_nextFenceValue(1)
	{

	}

	CommandQueue::~CommandQueue()
	{
		ShutDown();
	}

	bool CommandQueue::IsComplete(UINT fenceValue) const
	{
		UINT currentCompletefFenceValue = m_fence->GetCompletedValue();
		return fenceValue <= currentCompletefFenceValue;
	}

	void CommandQueue::Create(ID3D12Device* device)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = m_type;
		queueDesc.NodeMask = 0;
		THROW_IF_FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
		THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

		m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		ASSERT(m_fenceEvent != NULL);
	}

	void CommandQueue::ShutDown()
	{
		m_commandQueue->Release();
		CloseHandle(m_fenceEvent);
	}


}
