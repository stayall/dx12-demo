#pragma once
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>

#include "Sence.h"
#include "SenceObject.h"

namespace stay::Sence
{
	class SenceParse
	{
	public:
		std::unique_ptr<Sence>  ParseSence(const char* filename);
		std::unique_ptr<Sence>  ParseSence(const std::string& filename);

	private:
		 std::unique_ptr<Mesh> ParseMesh(aiMesh* mesh, const aiScene* scene);
		 std::shared_ptr<MeshNode> ParseNode(aiNode* node, const aiScene* scene, std::shared_ptr<Geometry>& geometry);

		 std::string GetErrorInfo() const { return m_importer.GetErrorString(); }
	private:
		static unsigned int kPostprocessFlag;

		Assimp::Importer m_importer;

	};
}