#pragma once

#include "App.h"
#include "../WinStd.h"

namespace stay::Display
{
	extern IDXGISwapChain3* g_SwapChain;

	void Initialize();
	void ShutDown();
}
