#pragma once

#include "../WinStd.h"
#include "CommandQueueManager.h"
#include "App.h"

namespace stay::Graphics
{
	extern ID3D12Device* g_Device;
	extern App* g_App;
	extern CommandQueueManager g_CommandManager;
	void Initialize();
	void Finalize();

}
