#pragma once

#include "../WinStd.h"
namespace stay
{
	class DescriptorHeap
	{
	public:

		struct DescriptorHeapHandle
		{
			DescriptorHeapHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHanlde = { CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN }, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = { CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN }) : m_cpuHanlde(cpuHanlde), m_gpuHanlde(gpuHandle) {};
			DescriptorHeapHandle operator+(const SIZE_T offset);
			DescriptorHeapHandle operator+=(const SIZE_T offset);

			operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_cpuHanlde; }
			operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_gpuHanlde; }

			D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHanlde;
			D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHanlde;

		};

		void Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask = 1);
		
		DescriptorHeapHandle Allcoate(UINT Count);
		DescriptorHeapHandle RetrieveAddress(UINT index);
		ID3D12DescriptorHeap& GetDescriptorHeap() { return *m_descriptorHeap.Get(); };

	protected:
		DescriptorHeapHandle m_startHanle;
		DescriptorHeapHandle m_currentHanlde;
		SIZE_T m_descriptorHandleIncrementSize = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	};

}
