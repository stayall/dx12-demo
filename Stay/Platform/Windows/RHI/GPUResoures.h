#pragma once

#include "../WinStd.h"
#include "../ClassAttribute.h"

namespace stay
{
	class GPUResoures : public NoCopyable
	{
	public:
		GPUResoures() { Clear(); };
		~GPUResoures() { Destory(); };

		GPUResoures(ID3D12Resource* resource,  D3D12_RESOURCE_STATES usageStatus = D3D12_RESOURCE_STATE_COMMON);
		virtual D3D12_RESOURCE_BARRIER_TYPE GetResourceBarrierType() { return D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; }
		virtual void Destory() { Clear(); };

		ID3D12Resource* GetResource() const { return m_resource.Get(); }
		D3D12_RESOURCE_STATES GetCurrentStatus() const { return m_usageStatus; }
		void SetCurrentStatus(D3D12_RESOURCE_STATES states) { m_usageStatus = states; }
		void SetTransitioningStatus(D3D12_RESOURCE_STATES states) { m_transitioningStatus = states; }
		D3D12_RESOURCE_STATES GetTransitioningStatus() const { return m_transitioningStatus; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return  m_gpuAddress; }
	protected:
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		void Clear();


		D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress;
		D3D12_RESOURCE_STATES m_transitioningStatus;
		D3D12_RESOURCE_STATES m_usageStatus;
		ComPtr<ID3D12Resource> m_resource;
	};

	class UploadBuffer : public GPUResoures
	{
	public:
		UploadBuffer() = default;
		~UploadBuffer() { Destory(); };

		void Create(const std::wstring& filename, size_t size, void* pData = nullptr);
		void* Map();
		void Unmap(size_t begin = 0u, size_t end = -1);
		size_t GetSize()  const { return m_size; }
	private:
		void* m_mapAddress = nullptr;
		size_t m_size;
	};


	class GPUBuffer : public GPUResoures
	{
	public:
		GPUBuffer() = default;
		~GPUBuffer() = default;

		void Create(const std::wstring& name);
		void Create(const std::wstring& name, size_t numElements, size_t elementSize, ID3D12Resource* pData = nullptr);
		void Create(const std::wstring& name, size_t numElements, size_t elementSize, UploadBuffer& uploadBuffer);

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(UINT offset, UINT SizeInBytes, UINT StrideInBytes) const;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(UINT SizeInBytes, UINT StrideInBytes) const { return VertexBufferView(0u, SizeInBytes, StrideInBytes); }
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(UINT offset, UINT SizeInBytes, DXGI_FORMAT Format = DXGI_FORMAT_R32_UINT) const;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(UINT SizeInBytes, DXGI_FORMAT Format = DXGI_FORMAT_R32_UINT) const { return IndexBufferView(0, SizeInBytes, Format); };

		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferView(UINT offset, UINT SizeInBytes);

	protected:
		size_t m_elementCount;
		size_t m_elementSize;
		size_t m_bufferSize;
	};
}
