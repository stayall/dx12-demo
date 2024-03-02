#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>  
#include <assimp/scene.h> 

#include <Platform/Windows/WinError.h>


#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

#include "SenceParse.h"

namespace  stay::Sence
{

	unsigned int SenceParse::kPostprocessFlag =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded;

	std::shared_ptr<Sence> SenceParse::ParseSence(const char* filename)
	{
		auto sence = std::make_shared<Sence>();

		const aiScene* aiScene = m_importer.ReadFile(filename, kPostprocessFlag);
		USER_ASSERT(aiScene != nullptr, m_importer.GetErrorString());

		if (aiScene->HasMaterials())
		{
			std::vector<std::shared_ptr<Material>> materials;
			for (size_t i = 0; i < aiScene->mNumMaterials; i++)
			{
				sence->m_materials.emplace_back()
			}
		}

		return ;
	}

	Mesh SenceParse::ParseMesh(aiMesh** meshs, unsigned int numMesh)
	{
		Mesh mesh{};
		for (unsigned int index = 0; index < numMesh; index++)
		{
			aiMesh* pMesh = meshs[index];

			if (pMesh->HasPositions())
			{
				mesh.AddVertexData(VertexArray::kPosition, reinterpret_cast<Math::Float*>(pMesh->mVertices), pMesh->mNumVertices * sizeof(aiVector3D) / sizeof(ai_real));
			}

			if (pMesh->HasNormals())
			{

			}

			std::vector<unsigned int> indices;
			for (size_t faceIndex = 0; faceIndex < pMesh->mNumFaces; faceIndex++)
			{
				const auto& face = pMesh->mFaces[faceIndex];
				for (size_t vertexIndex = 0; vertexIndex < face.mNumIndices; vertexIndex++)
				{
					indices.emplace_back(face.mIndices[vertexIndex]);
				}
			}
			mesh.AddVertexIndex(indices, indices.size());

		}

		return std::move(mesh);
	}
}
