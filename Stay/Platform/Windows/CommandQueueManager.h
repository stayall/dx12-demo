#ifndef _STAY_COMMAND_MANAGER_H_
#define _STAY_COMMAND_MANAGER_H_

#include "WinStd.h"
#include "WinError.h"

#include "CommandListManager.h"

namespace stay
{

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
		void ExecuteCommandList(ID3D12CommandList* commandList);
		void ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* commandLists);

		void CreateCommandList(const D3D12_COMMAND_LIST_TYPE commandListType, CommandList* commandList);
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

		CommandQueue& GetGraphicsQueue() { return m_graphicsQueue; };
	private:
		CommandQueue m_graphicsQueue;
	};

}



#endif // !_STAY_COMMAND_MANAGER_H_

