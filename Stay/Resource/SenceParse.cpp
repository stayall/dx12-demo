#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>  
#include <assimp/scene.h> 

#include <Platform/Windows/WinError.h>
#include <Core/Memory/MemoryManager.h>

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

#include "SenceParse.h"
#include "SenceObject.h"


namespace  stay::Sence
{

	unsigned int SenceParse::kPostprocessFlag =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded;

	std::unique_ptr<Sence>  SenceParse::ParseSence(const char* filename)
	{
		auto sence = std::unique_ptr<Sence>();

		const aiScene* aiScene = m_importer.ReadFile(filename, kPostprocessFlag);
		USER_ASSERT(aiScene != nullptr, m_importer.GetErrorString());

		if (aiScene->HasMaterials())
		{
			std::vector<std::shared_ptr<Material>> materials;
			for (size_t i = 0; i < aiScene->mNumMaterials; i++)
			{
				sence->m_materials.emplace_back();
			}
		}

		auto geometry = std::make_shared<Geometry>();
		for (size_t i = 0; i < aiScene->mNumMeshes; i++)
		{
			aiMesh* mesh = aiScene->mMeshes[i];
			geometry->AddMesh(ParseMesh(mesh, aiScene));
		}

		geometry->SetRoot(ParseNode(aiScene->mRootNode, aiScene, geometry));
		sence->m_geometries.push_back(std::move(geometry));
		

		return sence;
	}

	std::unique_ptr<Sence> SenceParse::ParseSence(const std::string& filename)
	{
		return ParseSence(filename.c_str());
	}

	std::unique_ptr<Mesh> SenceParse::ParseMesh(aiMesh* mesh, const aiScene* scene)
	{

		auto parseMesh = std::make_unique<Mesh>();
		parseMesh->AddVertexData(VertexDataType::kPosition, reinterpret_cast<Math::Float*>(mesh->mVertices), mesh->mNumVertices);
		if (mesh->mTextureCoords[0])
		{
			parseMesh->AddVertexData(VertexDataType::kUV, reinterpret_cast<Math::Float*>(mesh->mTextureCoords[0]), mesh->mNumVertices);
		}

		parseMesh->AddVertexData(VertexDataType::kTangent, reinterpret_cast<Math::Float*>(mesh->mTangents), mesh->mNumVertices);


		auto indices = std::vector<Math::UINT>(mesh->mNumFaces * 3);
		size_t numIndices = 0;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices[numIndices++] = face.mIndices[j];
		}
		parseMesh->AddVertexIndex(indices, numIndices, mesh->mMaterialIndex);

		return parseMesh;
	}


	std::shared_ptr<MeshNode> SenceParse::ParseNode(aiNode* node, const aiScene* aiScene, std::shared_ptr<Geometry>& geometry)
	{
		auto meshNode = std::make_shared<MeshNode>(node->mName.C_Str());
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = aiScene->mMeshes[node->mMeshes[i]];
			//geometry->AddMesh(ParseMesh(mesh, aiScene));
		}
		// 在我们处理完所有网格（如果有的话）后，我们会递归处理每个子节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			meshNode->AddChild(ParseNode(node->mChildren[i], aiScene, geometry));
		}
		
		return meshNode;
	}
}
