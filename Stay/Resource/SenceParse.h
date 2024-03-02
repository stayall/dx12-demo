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
		 std::shared_ptr<Sence> ParseSence(const char* filename);
		 Mesh ParseMesh(aiMesh** meshs, unsigned int numMesh);

		 std::string GetErrorInfo() const { return m_importer.GetErrorString(); }

	private:
		static unsigned int kPostprocessFlag;

		Assimp::Importer m_importer;

	};
}