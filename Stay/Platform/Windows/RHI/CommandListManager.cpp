#include "../WinError.h"

#include "CommandListManager.h"

#include "PipelineState.h"
#include "GraphicsCore.h"
#include "RootSignature.h"
#include "GPUResoures.h"

namespace stay
{
	CommandListManager::CommandListManager()
	{
	}

	CommandList* CommandListManager::AllocateCommandList(D3D12_COMMAND_LIST_TYPE type)
	{
		if (m_readyCommandList[type].empty())
		{
			m_commandListPool[type].emplace_back(std::make_unique<CommandList>());
			auto& commandList = m_commandListPool[type].back();
			commandList->Initialize(type);
			return commandList.get();
		}

		auto& commandList = m_readyCommandList[type].front();
		m_readyCommandList[type].pop();

		ASSERT(commandList->GetType() == type);

		return commandList;

	}

	void CommandListManager::FreeCommandList(CommandList* commandList)
	{
		commandList->Reset();
		m_readyCommandList[commandList->GetType()].push(commandList);
	}


	CommandList::~CommandList()
	{
		SAFE_RELEASE(m_commandList);
	}

	CommandList* CommandList::Begin(const std::wstring& name)
	{
		auto commandList = InternalCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		commandList->SetName(name);
		return commandList;
	}

	void CommandList::InitializeBuffer(const std::wstring& name, ID3D12Resource* pDstBuffer, ID3D12Resource* pSrcBuffer)
	{
		auto pCommandList = Begin(name);
		pCommandList->CopyResource(pDstBuffer, pSrcBuffer);
	}


	void CommandList::Initialize(D3D12_COMMAND_LIST_TYPE type)
	{
		m_type = type;
		Graphics::g_CommandManager.CreateCommandList(type, &m_commandList, &m_commandAllocator);
	}

	UINT64 CommandList::Flush(bool isWait)
	{
		if (m_type == D3D12_COMMAND_LIST_TYPE_BUNDLE)
		{
			return 0;
		}

		THROW_IF_FAILED(m_commandList->Close());
		auto& commandQueue = Graphics::g_CommandManager.GetQueue(m_type);
		UINT64 fenceValue = commandQueue.ExecuteCommandList(m_commandList);

		if (isWait)
		{
			commandQueue.WaitForFence(fenceValue);
		}

		return fenceValue;

	}

	UINT64 CommandList::Finsh(bool isWait)
	{
		if (m_type == D3D12_COMMAND_LIST_TYPE_BUNDLE)
		{
			return 0;
		}

		THROW_IF_FAILED(m_commandList->Close());

		auto& commandQueue = Graphics::g_CommandManager.GetQueue(m_type);
		UINT64 fenceValue = commandQueue.ExecuteCommandList(m_commandList);

		commandQueue.DiscardCommandListAllocator(fenceValue, m_commandAllocator);
		if (isWait)
		{
			commandQueue.WaitForFence(fenceValue);
		}

		Graphics::g_CommandListManager.FreeCommandList(this);
		return fenceValue;
	}


	void CommandList::Finalize()
	{
		Graphics::g_CommandListManager.FreeCommandList(this);
	}

	void CommandList::Reset(PSO* pso)
	{
		THROW_IF_FAILED(m_commandAllocator->Reset());
		if (pso == nullptr)
		{
			m_commandList->Reset(m_commandAllocator, nullptr);
			m_rootSignature = nullptr;
			m_pipelineState = nullptr;
			return;
		}

		m_commandList->Reset(m_commandAllocator, pso->GetPipelineState());
		m_rootSignature = (pso->GetRootSignature())->GetRootSignature();
		m_commandList->SetGraphicsRootSignature(m_rootSignature);
		m_pipelineState = pso->GetPipelineState();
	}

	void CommandList::SetName(const std::wstring& name)
	{
#if _RELEASE
		(name);
#else
		m_commandList->SetName(name.c_str());
#endif // _RELEASE

	}

	void CommandList::CopyBufferRegion(ID3D12Resource* pDstBuffer, UINT64 DstOffset, ID3D12Resource* pSrcBuffer, UINT64 SrcOffset, UINT64 NumBytes)
	{
		ASSERT(pSrcBuffer != nullptr && pDstBuffer != nullptr);
		m_commandList->CopyBufferRegion(pDstBuffer, DstOffset, pSrcBuffer, SrcOffset, NumBytes);
	}

	void CommandList::CopyResource(ID3D12Resource* pDstBuffer, ID3D12Resource* pSrcBuffer)
	{
		//FIEXD: 验证Resource Desc正确性
		ASSERT(pSrcBuffer != nullptr && pDstBuffer != nullptr);
		m_commandList->CopyResource(pDstBuffer, pSrcBuffer);
	}

	void CommandList::ResourceBarrier(GPUResoures* gpuResource, D3D12_RESOURCE_STATES nextResourceStates, BOOL isImmediatelyTransition)
	{
		if (gpuResource->GetCurrentStatus() == nextResourceStates || gpuResource->GetTransitioningStatus() == nextResourceStates)
		{
			return;
		}

		if (isImmediatelyTransition)
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transitionBarrier{};
			transitionBarrier.pResource = gpuResource->GetResource();
			transitionBarrier.StateBefore = gpuResource->GetCurrentStatus();
			transitionBarrier.StateAfter = nextResourceStates;

			D3D12_RESOURCE_BARRIER  rtvBarrier{};
			rtvBarrier.Type = gpuResource->GetResourceBarrierType();
			rtvBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			rtvBarrier.Transition = transitionBarrier;
			m_commandList->ResourceBarrier(1, &rtvBarrier);

			gpuResource->SetCurrentStatus(nextResourceStates);
			gpuResource->SetTransitioningStatus(nextResourceStates);
			return;
		}

		ASSERT(m_numsTransitionBarrier < sc_NumMaxBarriers);

		gpuResource->SetTransitioningStatus(nextResourceStates);
		m_transitionResource[m_numsTransitionBarrier] = gpuResource;
		++m_numsTransitionBarrier;

		if (m_numsTransitionBarrier >= sc_NumMaxBarriers)
		{
			FlushAllBarrier();
		}

	}

	void CommandList::ResourceBarrier(ID3D12Resource* resoures, D3D12_RESOURCE_STATES currentResourceStates, D3D12_RESOURCE_STATES nextResourceStates)
	{
		if (currentResourceStates == nextResourceStates)
		{
			return;
		}

		GPUResoures gpuResoures(resoures, currentResourceStates, nextResourceStates);
		ResourceBarrier(&gpuResoures, nextResourceStates, true);
	}


	void CommandList::FlushAllBarrier()
	{
		for (size_t i = 0; i < m_numsTransitionBarrier; i++)
		{
			auto gpuResource = m_transitionResource[i];

			D3D12_RESOURCE_TRANSITION_BARRIER transitionBarrier{};
			transitionBarrier.pResource = gpuResource->GetResource();
			transitionBarrier.StateBefore = gpuResource->GetCurrentStatus();
			transitionBarrier.StateAfter = gpuResource->GetTransitioningStatus();

			m_transitionBarriers[i].Transition = std::move(transitionBarrier);
			m_transitionBarriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			m_transitionBarriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			m_transitionBarriers[i].Transition = transitionBarrier;
		}

		m_commandList->ResourceBarrier(m_numsTransitionBarrier, m_transitionBarriers);

		for (size_t i = 0; i < m_numsTransitionBarrier; i++)
		{
			auto gpuResource = m_transitionResource[i];
			gpuResource->SetCurrentStatus(gpuResource->GetTransitioningStatus());
		}

		m_numsTransitionBarrier = 0u;
	}

	void CommandList::ExecuteBundle(CommandList &pCommandList)
	{
		m_commandList->ExecuteBundle(pCommandList.GetCommandList());
	}

	void CommandList::ExecuteBundle(ID3D12GraphicsCommandList* pCommandList)
	{
		m_commandList->ExecuteBundle(pCommandList);
	}


	CommandList* CommandList::InternalCommandList(D3D12_COMMAND_LIST_TYPE type)
	{
		return Graphics::g_CommandListManager.AllocateCommandList(type);
	}

	GraphicsCommandList* CommandList::GetGraphicsCommandList()
	{
		return reinterpret_cast<GraphicsCommandList*>(InternalCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT));
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

	GraphicsCommandList* GraphicsCommandList::Begin(const std::wstring& name)
	{
		auto commandList = GetGraphicsCommandList();
		commandList->SetName(name);
		return commandList;
	}


	void GraphicsCommandList::SetGraphicsRootSignature(const RootSignature& pso)
	{
		m_rootSignature = pso.GetRootSignature();
		m_commandList->SetGraphicsRootSignature(m_rootSignature);
	}
	void GraphicsCommandList::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
	{
		m_commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, baseDescriptor);
	}
	void GraphicsCommandList::SetPipelineState(const GraphicsPSO& pso)
	{
		m_pipelineState = pso.GetPipelineState();
		m_commandList->SetPipelineState(m_pipelineState);
	}
	void GraphicsCommandList::SetViewPort(float width, float height, float maxDepth, float minDepth)
	{
		D3D12_VIEWPORT viewPort{ width, height, maxDepth, minDepth };
		m_commandList->RSSetViewports(1u, &viewPort);
	}

	void GraphicsCommandList::SetScissorRect(LONG left, LONG top, LONG height, LONG width)
	{
		RECT rect{ left, top, height, width };
		m_commandList->RSSetScissorRects(1u, &rect);
	}
	void GraphicsCommandList::SetRenderTargets(UINT NumRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
	{
		m_commandList->OMSetRenderTargets(NumRenderTargetDescriptors, pRenderTargetDescriptors, FALSE, dsv);
	}
	void GraphicsCommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT colorRGBA[4])
	{
		m_commandList->ClearRenderTargetView(renderTargetView, colorRGBA, 0u, nullptr);
	}
}
