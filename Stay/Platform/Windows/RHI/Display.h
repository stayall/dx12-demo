#pragma once

#include "App.h"
#include "../WinStd.h"

namespace stay::Display
{
	const UINT sc_RenderTagetCount = 2;

	//extern ID3D12Resource* g_renderTarget[sc_RenderTagetCount];
	extern IDXGISwapChain3* g_SwapChain;
	//extern DescriptorHeap g_rtvDescriptorHeap; 
	//extern UINT g_freamIndex;

	void Initialize();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorHandle();
	ID3D12Resource* GetCurrentRenderTarget();
	void Swap();
	void ShutDown();
}
