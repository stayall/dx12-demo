#ifndef _STAY_GRAPHICS_H_
#define _STAY_GRAPHICS_H_

#include "WinStd.h"
#include "Window.h"
#include "WinError.h"
#include "WinHelper.h"

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

namespace stay
{
	class Graphics
	{
	public:
		Graphics(Window* window);

		class DescriptorHeap : public ID3D12DescriptorHeap
		{

		};

		class CommandAllocator : public ID3D12CommandAllocator
		{

		};

		class RootSignature : public ID3D12RootSignature
		{

		};

		class PipelineState : ID3D12PipelineState
		{

		};


		static void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);

	private:
		Window* pWindow;
		UINT width;
		UINT height;
		UINT freamCount = 2;
		UINT freamIndex;

		Microsoft::WRL::ComPtr<IDXGIFactory4> pFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapChian3;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDecriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget[2];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAlloc;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignatrue;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pPiplelineState;
		Microsoft::WRL::ComPtr<ID3D12Resource> verticeDataResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		UINT fenceValue = 0;
		Microsoft::WRL::ComPtr<ID3D12Fence> pFence;
		HANDLE m_fenceEvent;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbHeap;
		UINT8* pCbV;
		Microsoft::WRL::ComPtr<ID3D12Resource> cbvRes;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pBundleCommandList;
	};
}



#endif // !_STAY_GRAPHICS_H_

