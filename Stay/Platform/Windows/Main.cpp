#include <iostream>

#include "WinStd.h"
#include "Window.h"
#include "WinError.h"
#include "WinHelper.h"
#include "App.h"


#include "ClassAttribute.h"

#include "d3dx12.h"

#include "Display.h"
#include "GraphicsCore.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "DescriptorHeap.h"
#include "CommandQueueManager.h"
#include "CommandListManager.h"

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



using namespace stay;

int width = 1280;
int height = 720;

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
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

			stay::Graphics::Initialize();
			pDevice = stay::Graphics::g_Device;
		}
		Display::Initialize();



		

		stay::DescriptorHeap rtvDescriptorHeap{};
		rtvDescriptorHeap.Create(Graphics::g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

		ComPtr<ID3D12Resource> renderTarget[2];
		{

			for (UINT i = 0; i < 2; i++)
			{
				THROW_IF_FAILED(Display::g_SwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i])));
				pDevice->CreateRenderTargetView(renderTarget[i].Get(), nullptr, rtvDescriptorHeap.Allcoate(1));
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

		stay::RootSignature rootSignature;
		ComPtr<ID3D12RootSignature> pRootSignatrue;
		{
			D3D12_DESCRIPTOR_RANGE1 dcrpRange[1];
			dcrpRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			dcrpRange->NumDescriptors = 1;
			dcrpRange->BaseShaderRegister = 0;
			dcrpRange->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			dcrpRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			dcrpRange->RegisterSpace = 0;

			stay::RootSignatureParameter parameters;
			parameters.AddDescriptorRange(dcrpRange, _countof(dcrpRange));
			parameters.AsDescriptorTable(0, 0);


			rootSignature.AddParameter(parameters);
			rootSignature.Finalize(Graphics::g_Device);

			pRootSignatrue = rootSignature.GetRootSignature();

		}

		stay::GraphicsPSO pso;
		ComPtr<ID3D12PipelineState> pPiplelineState;
		{

			pso.SetRootSignature(&rootSignature);

			ComPtr<ID3DBlob> vertexShdaer;
			ComPtr<ID3DBlob> piexlShader;
			ComPtr<ID3DBlob> error;


			THROW_IF_FAILED(D3DReadFileToBlob(L"CompiledShaders/VSshaders.cso", &vertexShdaer));
			THROW_IF_FAILED(D3DReadFileToBlob(L"CompiledShaders/PSshaders.cso", &piexlShader));
			pso.SetVertexShader({ vertexShdaer->GetBufferPointer(), vertexShdaer->GetBufferSize() });
			pso.SetPiexlShader({ piexlShader->GetBufferPointer(), piexlShader->GetBufferSize() });

			D3D12_INPUT_ELEMENT_DESC inputDecs[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
			pso.SetInputLayout(inputDecs, _countof(inputDecs));
			pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			pso.SetRasterizerState(CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
			pso.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
			pso.SetRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM);
			pso.Fnalize(Graphics::g_Device);

			pPiplelineState = pso.GetPipelineState();
		}

	

		CommandList commandList;
		Graphics::g_CommandManager.GetGraphicsQueue().CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &commandList);
		auto pCommandList = commandList.GetCommandList();
		//pCommandList->SetPipelineState(pso.GetPipelineState());
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

	


		stay::DescriptorHeap cbvDescriptorHeap;
		cbvDescriptorHeap.Create(Graphics::g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1u, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

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


			pDevice->CreateConstantBufferView(&cbvDesc, cbvDescriptorHeap.Allcoate(1));
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




		UINT freamIndex = Display::g_SwapChain->GetCurrentBackBufferIndex();
		stay::App app;
		while (!app.CheckMessage())
		{

			{
				offset.offset += 0.01;
				memcpy(pCbV, &offset, sizeof(offset));
			}
			{
				THROW_IF_FAILED(pCommandAlloc.Reset());
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

				ID3D12DescriptorHeap* ppHeaps[] = { &cbvDescriptorHeap.GetDescriptorHeap()};
				pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
				pCommandList->SetGraphicsRootDescriptorTable(0, cbvDescriptorHeap.RetrieveAddress(0));

				D3D12_RESOURCE_TRANSITION_BARRIER transitionBarrier{};
				transitionBarrier.pResource = renderTarget[freamIndex].Get();
				transitionBarrier.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				transitionBarrier.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				D3D12_RESOURCE_BARRIER  rtvBarrier{};
				rtvBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				rtvBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				rtvBarrier.Transition = transitionBarrier;
				pCommandList->ResourceBarrier(1, &rtvBarrier);


				auto handle = (D3D12_CPU_DESCRIPTOR_HANDLE)rtvDescriptorHeap.RetrieveAddress(freamIndex);
				pCommandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);


				FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
				pCommandList->ClearRenderTargetView(rtvDescriptorHeap.RetrieveAddress(freamIndex), clearColor, 0, nullptr);

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
			auto pCommandQueue = Graphics::g_CommandManager.GetGraphicsQueue();
			CommandList cls[] = { commandList };
			pCommandQueue.ExecuteCommandLists(_countof(cls), cls);
			THROW_IF_FAILED(Display::g_SwapChain->Present(1, 0));

			pCommandQueue.WaitLastComplate();

			freamIndex = Display::g_SwapChain->GetCurrentBackBufferIndex();
		}
	}
	catch (const stay::Exception& e)
	{
		MessageBoxA(nullptr, e.what(), e.getType(), 0);
		OutputDebugStringA(e.what());
	}
	return 0;
}




