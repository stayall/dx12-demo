#ifndef _STAY_COMMAND_MANAGER_H_
#define _STAY_COMMAND_MANAGER_H_

#include "WinStd.h"
#include "WinError.h"

#include "CommandListManager.h"

namespace stay
{
	class CommandAllocator;
	class GraphicsPSO;
	class CommandList;

	class CommandQueue
	{
		friend class CommandQueueManager;
	public:
		CommandQueue();
		~CommandQueue();

		void Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		bool IsComplete(UINT64 fenceValue);
		void WaitForFence(UINT64 fenceValue);
		void WaitLastComplate();

		ID3D12CommandAllocator* RequierCommandListAllocator();
		void DiscardCommandListAllocator(ID3D12CommandAllocator* allocator);
		void ExecuteCommandList(ID3D12CommandList* const commandList);
		void ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* const *commandLists);

		void ShutDown();

		ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue; }
	private:
		static const size_t sc_NumMaxExcuteCommandList;

		D3D12_COMMAND_LIST_TYPE m_type;
		ID3D12CommandQueue* m_commandQueue;
		CommandAllocatorPool m_allocatorPool;
		ID3D12Device* m_device;
		ID3D12Fence* m_fence;
		HANDLE m_fenceEvent;
		UINT64 m_nextFenceValue;
		UINT64 m_lastCompleteFenceValue;

	};


	class CommandQueueManager
	{
	public:
		CommandQueueManager() = default;
		~CommandQueueManager() = default;

		void Initialize(ID3D12Device* device);

		void Finalize();

		void CreateCommandList(const D3D12_COMMAND_LIST_TYPE commandListType, ID3D12GraphicsCommandList** commandList, ID3D12CommandAllocator** allocator, GraphicsPSO* pso = nullptr, UINT nodeMask = 1);

		CommandQueue& GetGraphicsQueue() { return m_graphicsQueue; };
	private:
		ID3D12CommandAllocator* GetAllocator(const D3D12_COMMAND_LIST_TYPE commandListType);
	private:
		CommandQueue m_graphicsQueue;
		ID3D12Device* m_device;
	};

}



#endif // !_STAY_COMMAND_MANAGER_H_

