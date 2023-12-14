#pragma once

#include <vector>
#include <queue>

#include "../WinStd.h"

namespace stay
{

	class CommandAllocator;
	class GraphicsPSO;
	class CommandQueue;

	class CommandList
	{
		friend CommandQueue;
	public:
		CommandList() = default;
		~CommandList() { SAFE_RELEASE(m_commandList); }

		D3D12_COMMAND_LIST_TYPE GetType() const { return m_commandList->GetType(); }

		void Create(ID3D12Device* device, const D3D12_COMMAND_LIST_TYPE commandListType, ID3D12CommandAllocator* commandAllco, GraphicsPSO* pipelineState = nullptr, UINT nodemask = 1);
		//void Create(ID3D12Device* device, const D3D12_COMMAND_LIST_TYPE commandListType, ID3D12CommandAllocator* commandAllco, PSO* pipelineState = nullptr, UINT nodemask = 1);

		ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList; }



	private:
		ID3D12GraphicsCommandList* m_commandList;
	};

}

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