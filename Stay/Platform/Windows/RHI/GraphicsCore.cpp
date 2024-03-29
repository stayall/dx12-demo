#include "../WinError.h"
#include "../WinStd.h"

#include "App.h"

#include "GraphicsCore.h"
#include "CommandQueueManager.h"

using namespace Microsoft::WRL;

namespace stay::Graphics
{
	ID3D12Device* g_Device;
	App* g_App;
	CommandQueueManager g_CommandManager;
	CommandListManager g_CommandListManager;

	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter);


	void Initialize()
	{

		g_App = new App();
		ComPtr<ID3D12Device> pDevice;
		{

			ComPtr<IDXGIFactory4> pFactory;
			{
#ifdef _DEBUG
				ComPtr<ID3D12Debug> debugController;
				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
				{
					debugController->EnableDebugLayer();
				}

				THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(pFactory.GetAddressOf())));
#else
				THROW_IF_FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(pFactory.GetAddressOf())));
#endif // _DEBUG

		}

			{

				ComPtr<IDXGIAdapter1> pAdapter;
				{

					bool useWarpadapter = false;
					if (useWarpadapter)
					{
						pFactory->EnumWarpAdapter(IID_PPV_ARGS(pAdapter.GetAddressOf()));

					}
					else
					{
						GetHardwareAdapter(pFactory.Get(), pAdapter.GetAddressOf(), false);
					}
				}
				THROW_IF_FAILED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pDevice.GetAddressOf())));
			}


#ifdef _DEBUG
			{
				ComPtr<ID3D12InfoQueue> InfoQueue;
				if (pDevice->QueryInterface(IID_PPV_ARGS(&InfoQueue)) == S_OK)
				{

					InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
					InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
					InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				}
			}
#endif // _DEBUG


	}
		g_Device = pDevice.Detach();

		g_CommandManager.Initialize(g_Device);
}

	void Finalize()
	{
		g_CommandManager.Finalize();
		SAFE_RELEASE(g_Device);
		if (g_App != nullptr)
		{
			delete g_App;
		}

	}

	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter)
	{
		using Microsoft::WRL::ComPtr;
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		//FIXED: use gpu 
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter)));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}

}
