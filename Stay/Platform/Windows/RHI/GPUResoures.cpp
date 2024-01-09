
#include "../WinError.h"


#include "GPUResoures.h"
#include "GraphicsCore.h"

namespace stay
{
	GPUResoures::GPUResoures(ID3D12Resource* resource, D3D12_RESOURCE_STATES usageStatus)
	{
		m_resource = resource;

		m_transitioningStatus = usageStatus;
		m_usageStatus = usageStatus;
	}

	void GPUResoures::Clear()
	{
		m_resource = nullptr;
		m_transitioningStatus = D3D12_RESOURCE_STATE_COMMON;
		m_usageStatus = D3D12_RESOURCE_STATE_COMMON;
		m_gpuAddress = (D3D12_GPU_VIRTUAL_ADDRESS)CPU_GPU_VIRTUAL_ADDRESS_NULL;
	}

	void UploadBuffer::Create(const std::wstring& filename, size_t size, void* pData )
	{
		m_size = size;
		m_usageStatus = D3D12_RESOURCE_STATE_COMMON;
		m_transitioningStatus = m_usageStatus;

		D3D12_HEAP_PROPERTIES heapPro{};
		heapPro.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;


		D3D12_RESOURCE_DESC rsDesc{};
		rsDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		rsDesc.Alignment = 0;
		rsDesc.Width = m_size;
		rsDesc.Height = 1;
		rsDesc.DepthOrArraySize = 1;
		rsDesc.MipLevels = 1;
		rsDesc.Format = DXGI_FORMAT_UNKNOWN;
		rsDesc.SampleDesc.Count = 1;
		rsDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


		THROW_IF_FAILED(
			Graphics::g_Device->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &rsDesc,
				m_usageStatus,
				nullptr, IID_PPV_ARGS(&m_resource)));

		m_gpuAddress = m_resource->GetGPUVirtualAddress();

		if (pData == nullptr)
		{
			return;
		}
		
		auto pBuffer = Map();

		memcpy(pBuffer, pData, m_size);

#ifdef _RELEASE
		(filename);
#elif _DEBUG
		m_resource->SetName(filename.c_str());
#endif // RELEASE

	}

	void* UploadBuffer::Map()
	{
		if (m_mapAddress != nullptr)
		{
			return m_mapAddress;
		}

		D3D12_RANGE mapRange = { 0, m_size };

		THROW_IF_FAILED(m_resource->Map(0, &mapRange, &m_mapAddress));
		return m_mapAddress;
	}

	void UploadBuffer::Unmap(size_t begin, size_t end)
	{
		D3D12_RANGE unmapRange = { begin, std::min(end, m_size) };

		m_resource->Unmap(0, &unmapRange);
	}

	void GPUBuffer::Create(const std::wstring& name)
	{
		(name);
	}



	void GPUBuffer::Create(const std::wstring& name, size_t numElements, size_t elementSize, UploadBuffer& uploadBuffer)
	{
		m_elementCount = numElements;
		m_elementSize = elementSize;
		m_bufferSize = m_elementCount * m_elementSize;

		m_usageStatus = D3D12_RESOURCE_STATE_COMMON;
		m_transitioningStatus = m_usageStatus;

		D3D12_HEAP_PROPERTIES heapPro{};
		heapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;


		D3D12_RESOURCE_DESC rsDesc{};
		rsDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		rsDesc.Alignment = 0;
		rsDesc.Width = m_bufferSize;
		rsDesc.Height = 1;
		rsDesc.DepthOrArraySize = 1;
		rsDesc.MipLevels = 1;
		rsDesc.Format = DXGI_FORMAT_UNKNOWN;
		rsDesc.SampleDesc.Count = 1;
		rsDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


		THROW_IF_FAILED(
			Graphics::g_Device->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &rsDesc,
				m_usageStatus,
				nullptr, IID_PPV_ARGS(&m_resource)));

		m_gpuAddress = m_resource->GetGPUVirtualAddress();

		
			CommandList::InitializeBuffer(L"initialize Buffer", *this, uploadBuffer);
		

#ifdef _RELEASE
		(name);
#elif _DEBUG
		m_resource->SetName(name.c_str());
#endif // RELEASE

	}

	D3D12_VERTEX_BUFFER_VIEW GPUBuffer::VertexBufferView(UINT offset, UINT SizeInBytes, UINT StrideInBytes) const
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = m_gpuAddress + offset;
		vertexBufferView.SizeInBytes = SizeInBytes;
		vertexBufferView.StrideInBytes = StrideInBytes;

		return vertexBufferView;
	}


	D3D12_INDEX_BUFFER_VIEW GPUBuffer::IndexBufferView(UINT offset, UINT SizeInBytes, DXGI_FORMAT Format) const
	{
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
		indexBufferView.BufferLocation = m_gpuAddress + offset;
		indexBufferView.SizeInBytes = SizeInBytes;
		indexBufferView.Format = Format;

		return indexBufferView;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC GPUBuffer::ConstantBufferView(UINT offset, UINT SizeInBytes)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = m_gpuAddress + offset;
		cbvDesc.SizeInBytes = SizeInBytes;

		//Graphics::g_Device->CreateConstantBufferView(&cbvDesc, nullptr);
		return D3D12_CONSTANT_BUFFER_VIEW_DESC();
	}


}
