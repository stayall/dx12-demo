#include "SenceObject.h"

#include "Core/Memory/MemoryManager.h"
#include "Platform/Windows/WinError.h"

#define BEGIN_NAMESPACE(xx) namespace xx{
#define END_NAMESPACE(xx) }

BEGIN_NAMESPACE(stay::Sence)


VertexArray::VertexArray(VertexDataType type, Math::Float* srcData, size_t size)
	: BaseSenceObject(SenceObjectType(SenceObjectType::kMeshVertex)),
	m_type(type),
	m_size(size)
{

	m_data = std::shared_ptr<Math::Float[]>(Memory::Malloc<Math::Float>(m_size));
	memcpy(m_data.get(), srcData, size);
}

VertexArray::VertexArray(VertexArray::VertexDataType type, std::shared_ptr<Math::Float[]>& srcData, size_t size)
	: BaseSenceObject(SenceObjectType(SenceObjectType::kMeshVertex)),
	m_type(type),
	m_size(size)
{
	m_data = srcData;
}

IndexArray::IndexArray(uint32_t* srcData, size_t count)
	: BaseSenceObject(SenceObjectType::kMeshIndex),
	m_size(count)
{
	m_materialIndex = static_cast<uint32_t>(-1);
	m_restartIndex = static_cast<uint32_t>(-1);
	m_data = std::shared_ptr<uint32_t[]>(Memory::Malloc<uint32_t>(m_size ));
	memcpy(m_data.get(), srcData, m_size);
}

IndexArray::IndexArray(const std::vector<uint32_t>& srcData, size_t count)
	: BaseSenceObject(SenceObjectType::kMeshIndex),
	m_size(count)
{
	m_data = std::shared_ptr<uint32_t[]>(Memory::Malloc<uint32_t>(m_size ));

	for (size_t index = 0; index < m_size; index++)
	{
		m_data[index] = srcData[index];

	}
}

IndexArray::IndexArray(std::shared_ptr<uint32_t[]>& srcData, size_t count)
	: BaseSenceObject(SenceObjectType::kMeshIndex),
	m_size(count)
{
	m_materialIndex = static_cast<uint32_t>(-1);
	m_restartIndex = static_cast<uint32_t>(-1);
	m_data = srcData;
}

Mesh::Mesh()
	: BaseSenceObject(SenceObjectType::kMesh)
{

}
float* Mesh::GetVertexData(size_t index, VertexArray::VertexDataType type) const
{
	ASSERT(index < m_numSubMesh);

	return m_vertexDatas[type][index].GetVertex();
}
size_t Mesh::GetVertexCount(size_t index) const
{
	ASSERT(index < m_numSubMesh);
	return m_vertexDatas[VertexArray::kPosition][index].GetVertexCount();
}
uint32_t* Mesh::GetIndexData(size_t index) const
{
	ASSERT(index < m_numSubMesh);

	return m_vertexIndice[index].GetVertexIndex();
}
size_t Mesh::GetIndexCount(size_t index) const
{
	ASSERT(index < m_numSubMesh);

	return m_vertexIndice[index].GetIndexCount();
}

void Texture::LoadTexture()
{
	if (m_image != nullptr)
	{
		return;
	}



}

END_NAMESPACE(stay::Sence)

