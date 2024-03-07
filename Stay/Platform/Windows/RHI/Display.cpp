#include "Display.h"
#include "../WinError.h"
#include "GraphicsCore.h"
#include "DescriptorHeap.h"
#include "CommandQueueManager.h"
#include "CommandListManager.h"



namespace stay::Display
{

	IDXGISwapChain3* g_SwapChain;
	DescriptorHeap g_rtvDescriptorHeap{};
	ID3D12Resource* g_renderTarget[sc_RenderTagetCount];
	 
	UINT g_freamIndex = 0;

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
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

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


		g_rtvDescriptorHeap.Create(Graphics::g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		

			for (UINT i = 0; i < sc_RenderTagetCount; i++)
			{
				THROW_IF_FAILED(Display::g_SwapChain->GetBuffer(i, IID_PPV_ARGS(g_renderTarget + i)));
				Graphics::g_Device->CreateRenderTargetView(g_renderTarget[i], nullptr, g_rtvDescriptorHeap.Allcoate(1));
			}

			//g_freamIndex = g_SwapChain->GetCurrentBackBufferIndex();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorHandle()
	{
		return g_rtvDescriptorHeap.RetrieveAddress(Display::g_freamIndex);
	}

	ID3D12Resource* GetCurrentRenderTarget()
	{
		return g_renderTarget[g_freamIndex];
	}




	void Swap()
	{
		g_freamIndex = g_SwapChain->GetCurrentBackBufferIndex();

		THROW_IF_FAILED(Display::g_SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

	}

	void ShutDown()
	{
		g_SwapChain->Release();

	}
}