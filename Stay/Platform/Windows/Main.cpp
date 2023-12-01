#include <iostream>

#include "WinStd.h"
#include "Window.h"
#include "WinError.h"
#include "WinHelper.h"
#include "App.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <filesystem>
#include "ClassAttribute.h"

#include "d3dx12.h"



namespace dx = DirectX;




Align(256) struct MatrixConstantBuffer
{
	float offset;
};

struct VertexData
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 color;
};


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


inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}

int width = 1280;
int height = 720;

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		stay::Window window(L"stay", width, height);
		using Microsoft::WRL::ComPtr;

#ifdef _DEBUG
		{
			D3D12_DEBUG_FEATURE debugFlag;
			debugFlag |=
				D3D12_DEBUG_FEATURE_ALLOW_BEHAVIOR_CHANGING_DEBUG_AIDS |
				D3D12_DEBUG_FEATURE_CONSERVATIVE_RESOURCE_STATE_TRACKING |
				D3D12_DEBUG_FEATURE_DISABLE_VIRTUALIZED_BUNDLES_VALIDATION;

			ComPtr<ID3D12Debug> pDebug;
			THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf())));
			pDebug->EnableDebugLayer();


		}

#endif


		ComPtr<IDXGIFactory4> pFactory;
		{
#ifdef _DEBUG

			THROW_IF_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(pFactory.GetAddressOf())));
#else
			THROW_IF_FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(pFactory.GetAddressOf())));
#endif // _DEBUG

		}

		ComPtr<ID3D12Device> pDevice;
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
			pDevice->QueryInterface(IID_PPV_ARGS(&InfoQueue));
			InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		}
#endif // _DEBUG

		ComPtr<ID3D12CommandQueue> pCommandQueue;
		{

			D3D12_COMMAND_QUEUE_DESC queueDesc{};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			THROW_IF_FAILED(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(pCommandQueue.GetAddressOf())));
		}

		ComPtr<IDXGISwapChain3> pSwapChian3;
		{
			ComPtr<IDXGISwapChain1> pSwapChian;

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.BufferCount = 2;
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.SampleDesc.Count = 1;

			THROW_IF_FAILED(pFactory->CreateSwapChainForHwnd(
				pCommandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
				window.getHWnd(),
				&swapChainDesc,
				nullptr,
				nullptr,
				&pSwapChian
			));

			/*DXGI_SWAP_CHAIN_DESC chainDesc{};
			chainDesc.BufferDesc.Width = 600;
			chainDesc.BufferDesc.Height = 400;
			chainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			chainDesc.BufferCount = 2;
			chainDesc.SampleDesc.Count = 1;
			chainDesc.SampleDesc.Quality = 0;
			chainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
			chainDesc.OutputWindow = window.getHWnd();
			chainDesc.Windowed = TRUE;

			THROW_IF_FAILED(pFactory->CreateSwapChain(pCommandQueue.Get(), &chainDesc, pSwapChian.GetAddressOf()));*/

			THROW_IF_FAILED(pSwapChian.As(&pSwapChian3));
		}


		ComPtr<ID3D12DescriptorHeap> pDecriptorHeap;
		{
			D3D12_DESCRIPTOR_HEAP_DESC dhDesc{};
			dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dhDesc.NumDescriptors = 2;

			THROW_IF_FAILED(pDevice->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(pDecriptorHeap.GetAddressOf())));
		}

		ComPtr<ID3D12Resource> renderTarget[2];
		{
			UINT dhIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH(pDecriptorHeap->GetCPUDescriptorHandleForHeapStart());
			D3D12_CPU_DESCRIPTOR_HANDLE  rtvHandle = { pDecriptorHeap->GetCPUDescriptorHandleForHeapStart() };
			for (UINT i = 0; i < 2; i++)
			{
				THROW_IF_FAILED(pSwapChian3->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i])));
				pDevice->CreateRenderTargetView(renderTarget[i].Get(), nullptr, rtvHandle);
				rtvHandle.ptr += INT64(dhIncrementSize);
			}
		}

		ComPtr<ID3D12CommandAllocator> pCommandAlloc;
		{
			THROW_IF_FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAlloc.GetAddressOf())));
		}

		ComPtr<ID3D12CommandAllocator> pBundleCommandAlloc;
		{
			THROW_IF_FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(pBundleCommandAlloc.GetAddressOf())));
		}

		ComPtr<ID3D12RootSignature> pRootSignatrue;
		{

			D3D12_FEATURE_DATA_ROOT_SIGNATURE rootFeature{};
			rootFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootFeature, sizeof(rootFeature)) == E_INVALIDARG)
			{
				rootFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}


			D3D12_DESCRIPTOR_RANGE1 dcrpRange[1];
			dcrpRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			dcrpRange->NumDescriptors = 1;
			dcrpRange->BaseShaderRegister = 0;
			dcrpRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			dcrpRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			dcrpRange->RegisterSpace = 0;

			
			D3D12_ROOT_DESCRIPTOR_TABLE1 rdt{};
			rdt.NumDescriptorRanges = _countof(dcrpRange);
			rdt.pDescriptorRanges = &dcrpRange[0];

			D3D12_ROOT_PARAMETER1 rp[1];
			rp->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rp->DescriptorTable = rdt;
			rp->ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			

			D3D12_ROOT_SIGNATURE_DESC1 rsDecs{};
			rsDecs.NumParameters = _countof(rp);
			rsDecs.pParameters = rp;
			rsDecs.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

			

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC vrsd;
			vrsd.Version = rootFeature.HighestVersion;
			vrsd.Desc_1_1 = rsDecs;

			



			ComPtr<ID3DBlob> signatrue;
			ComPtr<ID3DBlob> error;
			THROW_IF_FAILED(D3D12SerializeVersionedRootSignature(&vrsd,  signatrue.GetAddressOf(), error.GetAddressOf()));

			THROW_IF_FAILED(pDevice->CreateRootSignature(0, signatrue->GetBufferPointer(), signatrue->GetBufferSize(), IID_PPV_ARGS(pRootSignatrue.GetAddressOf())));
		}

		ComPtr<ID3D12PipelineState> pPiplelineState;
		{


			UINT complierFlag = 0;
#ifdef _DEBUG
			complierFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

			ComPtr<ID3DBlob> vertexShdaer;
			ComPtr<ID3DBlob> piexlShader;
			ComPtr<ID3DBlob> error;
			wchar_t path[512];
			GetAssetsPath(path, 512);


			std::wstring pathFile(path);
			pathFile += L"shaders.hlsl";
			//D3DWriteBlobToFile(nullptr, L"./resss", true);
			THROW_IF_FAILED(D3DReadFileToBlob(L"CompiledShaders/VSshaders.cso", &vertexShdaer));
			THROW_IF_FAILED(D3DReadFileToBlob(L"CompiledShaders/PSshaders.cso", &piexlShader));
			//THROW_IF_FAILED(D3DCompileFromFile(pathFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", complierFlag, 0, vertexShdaer.GetAddressOf(), error.GetAddressOf()));
			//THROW_IF_FAILED(D3DCompileFromFile(pathFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", complierFlag, 0, piexlShader.GetAddressOf(), error.GetAddressOf()));


			D3D12_INPUT_ELEMENT_DESC inputDecs[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc{};
			plsDesc.pRootSignature = pRootSignatrue.Get();
			plsDesc.InputLayout = { inputDecs , _countof(inputDecs) };
			plsDesc.VS = { vertexShdaer->GetBufferPointer(), vertexShdaer->GetBufferSize() };
			plsDesc.PS = { piexlShader->GetBufferPointer(), piexlShader->GetBufferSize() };
			plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			plsDesc.SampleMask = UINT_MAX;
			plsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			plsDesc.DepthStencilState.DepthEnable = FALSE;
			plsDesc.DepthStencilState.StencilEnable = FALSE;
			plsDesc.SampleDesc.Count = 1;
			plsDesc.NumRenderTargets = 1;
			plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			plsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

			THROW_IF_FAILED(pDevice->CreateGraphicsPipelineState(&plsDesc, IID_PPV_ARGS(&pPiplelineState)));

		}

		ComPtr<ID3D12GraphicsCommandList> pCommandList;
		THROW_IF_FAILED(pDevice->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			pCommandAlloc.Get(),
			pPiplelineState.Get(),
			IID_PPV_ARGS(&pCommandList)));

		THROW_IF_FAILED(pCommandList->Close());

		VertexData verticeData[]
		{
			 { { 0.0f, 0.25f , 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f , 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f , 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		ComPtr<ID3D12Resource> verticeDataResource;
		{
			D3D12_HEAP_PROPERTIES heapPro{};
			heapPro.Type = D3D12_HEAP_TYPE_UPLOAD;
			heapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;


			D3D12_RESOURCE_DESC rsDesc{};
			rsDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			rsDesc.Alignment = 0;
			rsDesc.Width = sizeof(verticeData);
			rsDesc.Height = 1;
			rsDesc.DepthOrArraySize = 1;
			rsDesc.MipLevels = 1;
			rsDesc.Format = DXGI_FORMAT_UNKNOWN;
			rsDesc.SampleDesc.Count = 1;
			rsDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;



			THROW_IF_FAILED(
				pDevice->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &rsDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr, IID_PPV_ARGS(&verticeDataResource)));

			UINT8* pVertexData;
			D3D12_RANGE range{};
			THROW_IF_FAILED(verticeDataResource->Map(0, &range, reinterpret_cast<void**>(&pVertexData)));
			memcpy(pVertexData, reinterpret_cast<void*>(verticeData), sizeof(verticeData));
			verticeDataResource->Unmap(0, nullptr);



		}

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = verticeDataResource->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = sizeof(verticeData);
		vertexBufferView.StrideInBytes = sizeof(VertexData);

		UINT fenceValue = 0;

		ComPtr<ID3D12Fence> pFence;
		THROW_IF_FAILED(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

		fenceValue = 1;
		auto m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		if (m_fenceEvent == nullptr)
		{
			THROW_LASTEXCEPTION();
		}



		ComPtr<ID3D12DescriptorHeap> cbHeap;
		{
			D3D12_DESCRIPTOR_HEAP_DESC cbhDesc{};
			cbhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			cbhDesc.NumDescriptors = 1;

			THROW_IF_FAILED(pDevice->CreateDescriptorHeap(&cbhDesc, IID_PPV_ARGS(cbHeap.GetAddressOf())));
		}
		
		MatrixConstantBuffer offset{};
		UINT8* pCbV;
		ComPtr<ID3D12Resource> cbvRes;
		{
			D3D12_HEAP_PROPERTIES cbvProp{};
			cbvProp.Type = D3D12_HEAP_TYPE_UPLOAD;

			D3D12_RESOURCE_DESC resDesc{};
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc.Width = sizeof(offset);
			resDesc.Height = 1;
			resDesc.DepthOrArraySize = 1;
			resDesc.MipLevels = 1;
			resDesc.Format = DXGI_FORMAT_UNKNOWN;
			resDesc.SampleDesc.Count = 1;
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			THROW_IF_FAILED(pDevice->CreateCommittedResource(
				&cbvProp, D3D12_HEAP_FLAG_NONE,
				&resDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&cbvRes)));

			D3D12_RANGE range{};
			cbvRes->Map(0, &range, reinterpret_cast<void**>(&pCbV));
			memcpy(pCbV, &offset, sizeof(offset));
		}


		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
			cbvDesc.BufferLocation = cbvRes->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = sizeof(offset);


			D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle(cbHeap->GetCPUDescriptorHandleForHeapStart());
			cbvDesc.BufferLocation = cbvRes->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = sizeof(offset);

			pDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);
		}

		ComPtr<ID3D12GraphicsCommandList> pBundleCommandList;
		THROW_IF_FAILED(pDevice->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_BUNDLE,
			pBundleCommandAlloc.Get(),
			pPiplelineState.Get(),
			IID_PPV_ARGS(&pBundleCommandList)));
		{
			pBundleCommandList->SetGraphicsRootSignature(pRootSignatrue.Get());
			pBundleCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pBundleCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			pBundleCommandList->DrawInstanced(3, 1, 0, 0);
			THROW_IF_FAILED(pBundleCommandList->Close());
		}




		UINT freamIndex = pSwapChian3->GetCurrentBackBufferIndex();
		stay::App app;
		while (!app.CheckMessage())
		{

			{
				offset.offset += 0.01;
				memcpy(pCbV, &offset, sizeof(offset));
			}
			{
				THROW_IF_FAILED(pCommandAlloc->Reset());
				THROW_IF_FAILED(pCommandList->Reset(pCommandAlloc.Get(), pPiplelineState.Get()));


				pCommandList->SetGraphicsRootSignature(pRootSignatrue.Get());

				D3D12_VIEWPORT viewPort{};
				viewPort.Width = (float)width;
				viewPort.Height = (float)height;
				viewPort.MaxDepth = 1;
				viewPort.MinDepth = 0;
				pCommandList->RSSetViewports(1, &viewPort);

				D3D12_RECT rect = { 0, 0, width, height };
				pCommandList->RSSetScissorRects(1, &rect);

				ID3D12DescriptorHeap* ppHeaps[] = { cbHeap.Get() };
				pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
				pCommandList->SetGraphicsRootDescriptorTable(0, cbHeap->GetGPUDescriptorHandleForHeapStart());

				D3D12_RESOURCE_TRANSITION_BARRIER transitionBarrier{};
				transitionBarrier.pResource = renderTarget[freamIndex].Get();
				transitionBarrier.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				transitionBarrier.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				D3D12_RESOURCE_BARRIER  rtvBarrier{};
				rtvBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				rtvBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				rtvBarrier.Transition = transitionBarrier;
				pCommandList->ResourceBarrier(1, &rtvBarrier);

				UINT incrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				D3D12_CPU_DESCRIPTOR_HANDLE rvtHandle(pDecriptorHeap->GetCPUDescriptorHandleForHeapStart());
				rvtHandle.ptr += INT64(freamIndex) * INT64(incrementSize);

				pCommandList->OMSetRenderTargets(1, &rvtHandle, FALSE, nullptr);


				FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
				pCommandList->ClearRenderTargetView(rvtHandle, clearColor, 0, nullptr);

				pCommandList->ExecuteBundle(pBundleCommandList.Get());

				transitionBarrier.pResource = renderTarget[freamIndex].Get();
				transitionBarrier.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				transitionBarrier.StateAfter = D3D12_RESOURCE_STATE_PRESENT;


				rtvBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				rtvBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				rtvBarrier.Transition = transitionBarrier;
				pCommandList->ResourceBarrier(1, &rtvBarrier);


				THROW_IF_FAILED(pCommandList->Close());
			}

			ID3D12CommandList* cls[] = { pCommandList.Get() };
			pCommandQueue->ExecuteCommandLists(_countof(cls), cls);
			THROW_IF_FAILED(pSwapChian3->Present(1, 0));

			const UINT fence = fenceValue;
			THROW_IF_FAILED(pCommandQueue->Signal(pFence.Get(), fence));
			fenceValue++;
			if (pFence->GetCompletedValue() < fence)
			{
				THROW_IF_FAILED(pFence->SetEventOnCompletion(fence, m_fenceEvent));
				WaitForSingleObject(m_fenceEvent, INFINITE);
			}

			freamIndex = pSwapChian3->GetCurrentBackBufferIndex();
		}
	}
	catch (const stay::Exception& e)
	{
		MessageBoxA(nullptr, e.what(), e.getType(), 0);
		OutputDebugStringA(e.what());
	}
	return 0;
}




