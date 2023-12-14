#include "../WinError.h"

#include "CommandListManager.h"
#include "PipelineState.h"

namespace stay
{

	void CommandList::Create(ID3D12Device* device, const D3D12_COMMAND_LIST_TYPE commandListType, ID3D12CommandAllocator* commandAllco, GraphicsPSO* pipelineState, UINT nodemask)
	{
		ASSERT(commandListType != D3D12_COMMAND_LIST_TYPE_COMPUTE);
		ASSERT(commandAllco != nullptr && device != nullptr);
		if (pipelineState != nullptr)
		{
			device->CreateCommandList(nodemask, commandListType, commandAllco, pipelineState->GetPipelineState(), IID_PPV_ARGS(&m_commandList));

		}
		else
		{
			device->CreateCommandList(nodemask, commandListType, commandAllco, nullptr, IID_PPV_ARGS(&m_commandList));
		}
		THROW_IF_FAILED(m_commandList->Close());
	}

}

void CommandAllocatorPool::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	m_device = device;
	m_type = type;
}

ID3D12CommandAllocator* CommandAllocatorPool::AllocateCommandAllocator(UINT64 fenceValue)
{
	ID3D12CommandAllocator* commandAllocator = nullptr;

	if (!m_readyCommandAllocator.empty())
	{
		auto& front = m_readyCommandAllocator.front();
		if (front.first <= fenceValue)
		{
			commandAllocator = front.second;
			commandAllocator->Reset();
		}
	}

	if (commandAllocator == nullptr)
	{
		THROW_IF_FAILED(m_device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&commandAllocator)));
		m_commandAllocatorPool.push_back(commandAllocator);
	}
	return commandAllocator;
}

void CommandAllocatorPool::DeallocateCommandAllocator(UINT64 FenceValue, ID3D12CommandAllocator* commandAllocator)
{
	m_readyCommandAllocator.push(std::make_pair(FenceValue, commandAllocator));
}

void CommandAllocatorPool::Destoty()
{
	for (size_t i = 0; i < m_commandAllocatorPool.size(); i++)
	{
		m_commandAllocatorPool[i]->Release();
	}
}
