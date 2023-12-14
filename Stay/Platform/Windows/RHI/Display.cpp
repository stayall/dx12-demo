#include "Display.h"
#include "../WinError.h"
#include "GraphicsCore.h"



namespace stay::Display
{
	IDXGISwapChain3* g_SwapChain;
	
	void Initialize()
	{
		using Microsoft::WRL::ComPtr;
		ComPtr<IDXGIFactory2> factory;
		CreateDXGIFactory2(0u, IID_PPV_ARGS(&factory));

		ComPtr<IDXGISwapChain1> swapChian;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = Graphics::g_App->GetWindow().getWidth();
		swapChainDesc.Height = Graphics::g_App->GetWindow().getHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		THROW_IF_FAILED(factory->CreateSwapChainForHwnd(
			Graphics::g_CommandManager.GetGraphicsQueue().GetCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
			Graphics::g_App->GetWindow().getHWnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChian
		));

		ComPtr<IDXGISwapChain3> swapChian3;

		THROW_IF_FAILED(swapChian.As(&swapChian3));
		g_SwapChain = swapChian3.Detach();
	}

	void ShutDown()
	{
		g_SwapChain->Release();

	}
}