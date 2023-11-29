#ifndef _STAY_COMMAND_MANAGER_H_
#define _STAY_COMMAND_MANAGER_H_

#include "WinStd.h"
#include "WinError.h"

namespace stay
{
	class CommandQueue
	{
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		bool IsComplete(UINT fenceValue) const;
		void Create(ID3D12Device* device);
		void ShutDown();
	private:
		D3D12_COMMAND_LIST_TYPE m_type;
		ID3D12CommandQueue* m_commandQueue;

		ID3D12Fence* m_fence;
		HANDLE m_fenceEvent;
		UINT m_nextFenceValue;
		UINT m_lastCompleteFenceValue;
	};
}

#endif // !_STAY_COMMAND_MANAGER_H_

