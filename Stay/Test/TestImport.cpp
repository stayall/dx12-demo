
#include <Platform/Windows/WinStd.h>
#include <Resource/SenceParse.h>


#include "Platform/Windows/RHI/Window.h"

#include "Platform/Windows/WinHelper.h"
#include "Platform/Windows/RHI/App.h"


#include "Platform/Windows/ClassAttribute.h"

#include "Platform/Windows/RHI/d3dx12.h"

#include "Platform/Windows/RHI/Display.h"
#include "Platform/Windows/RHI/GraphicsCore.h"
#include "Platform/Windows/RHI/RootSignature.h"
#include "Platform/Windows/RHI/PipelineState.h"
#include "Platform/Windows/RHI/DescriptorHeap.h"

#include "Platform/Windows/RHI/GPUResoures.h"

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

	stay::Sence::SenceParse senceParse;

	auto Sence =  senceParse.ParseSence("../Asset/fu.fbx");

	using Microsoft::WRL::ComPtr;
	stay::Graphics::Initialize();
	Display::Initialize();

	stay::DescriptorHeap rtvDescriptorHeap{};
	rtvDescriptorHeap.Create(Graphics::g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	ComPtr<ID3D12Resource> renderTarget[2];
	{

		for (UINT i = 0; i < 2; i++)
		{
			THROW_IF_FAILED(Display::g_SwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i])));
			Graphics::g_Device->CreateRenderTargetView(renderTarget[i].Get(), nullptr, rtvDescriptorHeap.Allcoate(1));
		}
	}


	ComPtr<ID3D12CommandAllocator> pCommandAlloc;
	{
		THROW_IF_FAILED(Graphics::g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAlloc.GetAddressOf())));
	}

	ComPtr<ID3D12CommandAllocator> pBundleCommandAlloc;
	{
		THROW_IF_FAILED(Graphics::g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(pBundleCommandAlloc.GetAddressOf())));
	}

	stay::RootSignature rootSignature;
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


	}

	stay::GraphicsPSO pso;
	{

		pso.SetRootSignature(&rootSignature);

		ComPtr<ID3DBlob> vertexShdaer;
		ComPtr<ID3DBlob> piexlShader;
		ComPtr<ID3DBlob> error;


		THROW_IF_FAILED(D3DReadFileToBlob(L"../CompiledShaders/VSshaders.cso", &vertexShdaer));
		THROW_IF_FAILED(D3DReadFileToBlob(L"../CompiledShaders/PSshaders.cso", &piexlShader));
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

	}




	//pCommandList->SetPipelineState(pso.GetPipelineState());
	VertexData verticeData[]
	{
		 { { 0.0f, 0.25f , 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f , 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f , 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	stay::UploadBuffer verticeDataRs;
	verticeDataRs.Create(L"Traingle Vertice Data", sizeof(verticeData), &verticeData);


	stay::GPUBuffer modelData{};

	modelData.Create(L"Model", 1u, verticeDataRs.GetSize(), verticeDataRs);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = modelData.VertexBufferView(sizeof(verticeData), sizeof(VertexData));



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

		THROW_IF_FAILED(Graphics::g_Device->CreateCommittedResource(
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


		Graphics::g_Device->CreateConstantBufferView(&cbvDesc, cbvDescriptorHeap.Allcoate(1));
	}

	ComPtr<ID3D12GraphicsCommandList> pBundleCommandList;
	THROW_IF_FAILED(Graphics::g_Device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_BUNDLE,
		pBundleCommandAlloc.Get(),
		pso.GetPipelineState(),
		IID_PPV_ARGS(&pBundleCommandList)));
	{
		pBundleCommandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
		pBundleCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pBundleCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		pBundleCommandList->DrawInstanced(3, 1, 0, 0);
		THROW_IF_FAILED(pBundleCommandList->Close());
	}




	UINT freamIndex = Display::g_SwapChain->GetCurrentBackBufferIndex();

	auto commandList = stay::GraphicsCommandList::Begin(L"Main Draw Model");
	while (!Graphics::g_App->CheckMessage())
	{

		{
			offset.offset += 0.01;
			memcpy(pCbV, &offset, sizeof(offset));
		}
		{

			commandList->SetPipelineState(pso);
			commandList->SetGraphicsRootSignature(rootSignature);

			D3D12_VIEWPORT viewPort{};
			viewPort.Width = (float)width;
			viewPort.Height = (float)height;
			viewPort.MaxDepth = 1;
			viewPort.MinDepth = 0;
			commandList->SetViewport(viewPort);

			D3D12_RECT rect = { 0, 0, width, height };
			commandList->SetScissorRect(rect);

			ID3D12DescriptorHeap* ppHeaps[] = { &cbvDescriptorHeap.GetDescriptorHeap() };
			commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
			commandList->SetGraphicsRootDescriptorTable(0, cbvDescriptorHeap.RetrieveAddress(0));


			commandList->ResourceBarrier(renderTarget[freamIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);


			auto handle = (D3D12_CPU_DESCRIPTOR_HANDLE)rtvDescriptorHeap.RetrieveAddress(freamIndex);
			commandList->SetRenderTargets(1, &handle, nullptr);

			FLOAT clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
			commandList->ClearRenderTargetView(rtvDescriptorHeap.RetrieveAddress(freamIndex), clearColor);

			commandList->ExecuteBundle(pBundleCommandList.Get());
			commandList->GetCommandList()->DrawInstanced(3, 1, 0, 0);


			commandList->ResourceBarrier(renderTarget[freamIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			commandList->Flush(true);
		}


		THROW_IF_FAILED(Display::g_SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

		freamIndex = Display::g_SwapChain->GetCurrentBackBufferIndex();
	}
}