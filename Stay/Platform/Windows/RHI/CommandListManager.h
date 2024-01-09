#pragma once

#include <vector>
#include <queue>
#include <memory>

#include "../WinStd.h"
#include "../ClassAttribute.h"



namespace stay
{
	class CommandList;

	class CommandListManager
	{
	public:
		CommandListManager();
		CommandList* AllocateCommandList(D3D12_COMMAND_LIST_TYPE type);
		void FreeCommandList(CommandList*);

	private:
		std::vector<std::unique_ptr<CommandList>> m_commandListPool[4];
		std::queue<CommandList*> m_readyCommandList[4];
	};




	class GraphicsPSO;
	class PSO; 
	class RootSignature;
	class GPUResoures;
	class CommandQueueManager;
	class GraphicsCommandList;

	class CommandList : public NoCopyable
	{
		friend class CommandListManager;
	public:
		CommandList() = default;
		~CommandList();

		static CommandList* Begin(const std::wstring& name);
		void Initialize(D3D12_COMMAND_LIST_TYPE type);
		UINT64 Flush(bool isWait);
		UINT64 Finsh(bool isWait);
		void Finalize();
		void Reset(PSO* pso = nullptr);



	public:
		static void InitializeBuffer(const std::wstring& name, GPUResoures& pDstBuffer, GPUResoures& pSrcBuffer);

		void SetName(const std::wstring& name);
		void SetDescriptorHeaps(UINT numDescriptorHeaps, ID3D12DescriptorHeap** const descriptorHeaps) { m_commandList->SetDescriptorHeaps(numDescriptorHeaps, descriptorHeaps); }
		void CopyBufferRegion(ID3D12Resource* pDstBuffer, UINT64 DstOffset, ID3D12Resource* pSrcBuffer, UINT64 SrcOffset, UINT64 NumBytes);
		void CopyResource(ID3D12Resource* pDstBuffer, ID3D12Resource* pSrcBuffer);

		void ResourceBarrier(GPUResoures* gpuResoures, D3D12_RESOURCE_STATES nextResourceStates, BOOL isImmediatelyTransition = false);
		void ResourceBarrier(ID3D12Resource* resoures, D3D12_RESOURCE_STATES currentResourceStates, D3D12_RESOURCE_STATES nextResourceStates);
		void FlushAllBarrier();

		void ExecuteBundle(CommandList &pCommandList);
		void ExecuteBundle(ID3D12GraphicsCommandList * pCommandList);

		D3D12_COMMAND_LIST_TYPE GetType() const { return m_type; };
		ID3D12GraphicsCommandList* GetCommandList()  const { return m_commandList; };
	protected:
		static CommandList* InternalCommandList(D3D12_COMMAND_LIST_TYPE type);
		static GraphicsCommandList* GetGraphicsCommandList();

	
	protected:
		D3D12_COMMAND_LIST_TYPE m_type;
		ID3D12GraphicsCommandList* m_commandList;
		ID3D12CommandAllocator* m_commandAllocator;
		ID3D12RootSignature* m_rootSignature;

		ID3D12PipelineState* m_pipelineState;

		static const UINT sc_NumMaxBarriers = 16;
		GPUResoures* m_transitionResource[sc_NumMaxBarriers];
		D3D12_RESOURCE_BARRIER m_transitionBarriers[sc_NumMaxBarriers];
		UINT m_numsTransitionBarrier;
	};


	class GraphicsCommandList : public CommandList
	{
	public:
		static GraphicsCommandList* Begin(const std::wstring& name = L"Unamed");


		void SetGraphicsRootSignature(const RootSignature& rs);
		void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
		void SetPipelineState(const GraphicsPSO& pso);

		void SetViewports(UINT numViewPort, const D3D12_VIEWPORT* viewPort) { m_commandList->RSSetViewports(numViewPort, viewPort); }
		void SetViewport(const D3D12_VIEWPORT& viewPort) { m_commandList->RSSetViewports(1u, &viewPort); }
		void SetViewPort(float width, float height, float maxDepth = 1.0f, float minDepth = 0.0f);

		void SetScissorRects(UINT numScissorRect, const RECT* ScissorRect) { m_commandList->RSSetScissorRects(numScissorRect, ScissorRect); }
		void SetScissorRect(const RECT& ScissorRect) { m_commandList->RSSetScissorRects(1u, &ScissorRect); }
		void SetScissorRect(LONG left, LONG top, LONG height, LONG width);


		void SetRenderTargets(UINT NumRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* dsv = nullptr);
		void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetDescriptors) { SetRenderTargets(1u, &RenderTargetDescriptors); }
		void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv) { SetRenderTargets(1u, &RenderTargetDescriptors, &dsv); }
		void SetDepthStencil(const D3D12_CPU_DESCRIPTOR_HANDLE& dsv) { SetRenderTargets(0u, nullptr, &dsv); }

		void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT[4]);

	private:
	};

	class CommandAllocatorPool
	{
	public:
		CommandAllocatorPool() = default;
		~CommandAllocatorPool() = default;
		void Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		ID3D12CommandAllocator* AllocateCommandAllocator(UINT64 fenceValue);
		void DeallocateCommandAllocator(UINT64 FenceValue, ID3D12CommandAllocator* commandAllocator);
		void Destoty();
	private:
		D3D12_COMMAND_LIST_TYPE m_type;
		ID3D12Device* m_device;
		std::vector<ID3D12CommandAllocator*> m_commandAllocatorPool;
		std::queue<std::pair<UINT64, ID3D12CommandAllocator*>> m_readyCommandAllocator;
	};
}