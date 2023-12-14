#include "DescriptorHeap.h"
#include "GraphicsCore.h"
#include "../WinError.h"
namespace stay
{
	void DescriptorHeap::Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask)
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbhDesc{};
		cbhDesc.Type = heapType;
		cbhDesc.Flags = flags;
		cbhDesc.NumDescriptors = numDescriptors;
		cbhDesc.NodeMask = nodeMask;
		THROW_IF_FAILED(device->CreateDescriptorHeap(&cbhDesc, IID_PPV_ARGS(&m_descriptorHeap)));
		if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
		{
			m_currentHanlde = { m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_GPU_DESCRIPTOR_HANDLE(CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN) };
		}
		else if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			m_currentHanlde = { m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_descriptorHeap->GetGPUDescriptorHandleForHeapStart() };
		}
		m_startHanle = m_currentHanlde;
		m_descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(heapType);

	}

	DescriptorHeap::DescriptorHeapHandle DescriptorHeap::Allcoate(UINT Count)
	{
		ASSERT(Count >= 0);

		DescriptorHeapHandle lastHandle = m_currentHanlde;
		m_currentHanlde += Count * m_descriptorHandleIncrementSize;
		return lastHandle;
	}

	DescriptorHeap::DescriptorHeapHandle DescriptorHeap::RetrieveAddress(UINT index)
	{
		UINT size = static_cast<UINT>((m_currentHanlde.m_cpuHanlde.ptr - m_startHanle.m_cpuHanlde.ptr) / m_descriptorHandleIncrementSize);
		ASSERT(index < size || (size == 0 && index == 0));

		return m_startHanle + index * m_descriptorHandleIncrementSize;

	}

	DescriptorHeap::DescriptorHeapHandle DescriptorHeap::DescriptorHeapHandle::operator+(const SIZE_T offset)
	{
		DescriptorHeapHandle result = { m_cpuHanlde , m_gpuHanlde };
		if (result.m_cpuHanlde.ptr != CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			result.m_cpuHanlde.ptr += offset;
		}

		if (result.m_gpuHanlde.ptr != CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			result.m_gpuHanlde.ptr += offset;
		}
		return result;
	}
	DescriptorHeap::DescriptorHeapHandle DescriptorHeap::DescriptorHeapHandle::operator+=(const SIZE_T offset)
	{
		if (m_cpuHanlde.ptr != CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			m_cpuHanlde.ptr += offset;
		}

		if (m_gpuHanlde.ptr != CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			m_gpuHanlde.ptr += offset;
		}
		return *this;
	}
}