#pragma once

#include <memory>
#include <vector>

#include <crossguid/guid.hpp>

#include <Core/Math/MathType.h>

namespace stay::Sence
{

	enum class SenceObjectType
	{
		Invaild = -1,
		kMesh,
		kMeshVertex,
		kMeshIndex,
		kTransform,
		kMaterial,
		kMap,
		kCamera,
		kLight
	};

	class BaseSenceObject
	{
	protected:
		BaseSenceObject(SenceObjectType type) : m_type(type) { m_guid = xg::newGuid(); }
		BaseSenceObject(const BaseSenceObject& rhs) = delete;
		BaseSenceObject& operator=(const BaseSenceObject& rhs) = delete;

		BaseSenceObject(BaseSenceObject&& rhs) = default;
		BaseSenceObject& operator=(BaseSenceObject&& rhs) = default;
	protected:
		SenceObjectType m_type;
		xg::Guid m_guid;
	};


	class VertexArray : public BaseSenceObject
	{
	public:
		enum VertexDataType
		{
			kPosition,
			kNormal,
			kTangent,
			kColor,
			kUV,
			kNumVertexDataType
		};

		VertexArray(VertexDataType type, Math::Float* srcData, size_t count);
		VertexArray(VertexDataType type, std::shared_ptr<Math::Float[]>& srcData, size_t count);

		size_t GetVertexCount() const { return m_size; }
		VertexDataType GetVertexDataType() const { return m_type; }

		Math::Float* GetVertex() const { return m_data.get(); }
	private:
		VertexDataType m_type;
		size_t m_size;

		std::shared_ptr<Math::Float[]> m_data;
	};

	class IndexArray : public BaseSenceObject
	{
	public:

		IndexArray(uint32_t* srcData, size_t count);
		IndexArray(const std::vector<uint32_t>& srcData, size_t count);
		IndexArray(std::shared_ptr<uint32_t[]>& srcData, size_t count);

		size_t GetIndexCount() const { return m_size; }
		uint32_t* GetVertexIndex() const { return m_data.get(); }
	private:
		uint32_t m_materialIndex;
		uint32_t m_restartIndex;

		size_t m_size;
		std::shared_ptr<uint32_t[]> m_data;
	};

	class Mesh : BaseSenceObject
	{
	public:
		Mesh();

		void AddVertexData(VertexArray::VertexDataType type, Math::Float* srcData, size_t count) { m_numSubMesh++; m_vertexDatas[type].emplace_back(type, srcData, count); }
		void AddVertexData(VertexArray::VertexDataType type, std::shared_ptr<Math::Float[]>& srcData, size_t count) { m_numSubMesh++; m_vertexDatas[type].emplace_back(type, srcData, count); }

		void AddVertexIndex(uint32_t* srcData, size_t count) { m_numSubMesh++; m_vertexIndice.emplace_back(srcData, count); }
		void AddVertexIndex(const std::vector<uint32_t>& srcData, size_t count) { m_vertexIndice.emplace_back(srcData, count); }
		void AddVertexIndex(std::shared_ptr<uint32_t[]>& srcData, size_t count) { m_vertexIndice.emplace_back(srcData, count); }

		size_t GetMeshCount() const { return m_numSubMesh; }

		float* GetVertexData(size_t index, VertexArray::VertexDataType type = VertexArray::VertexDataType::kPosition) const;
		size_t GetVertexCount(size_t index) const;

		uint32_t* GetIndexData(size_t index) const;
		size_t GetIndexCount(size_t index) const;
	private:
		size_t m_numSubMesh = 0;
		std::vector<VertexArray> m_vertexDatas[VertexArray::VertexDataType::kNumVertexDataType];
		std::vector<IndexArray> m_vertexIndice;
	};
}