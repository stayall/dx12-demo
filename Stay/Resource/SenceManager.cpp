#include "SenceManager.h"

namespace stay::Resource
{
	SenceManager::SenceManager()
	{
	}

	SenceManager::~SenceManager()
	{
	}

	void SenceManager::LoadSence(const std::string& senceName)
	{
		m_sence = m_senceParse.ParseSence(senceName);
	}
}

