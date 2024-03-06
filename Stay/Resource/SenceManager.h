#pragma once

#include "Sence.h"
#include "SenceParse.h"

namespace stay::Resource
{
	class SenceManager
	{
	public:
		SenceManager();
		~SenceManager();

		void LoadSence(const std::string &senceName);

		Sence::Sence& GetRenderSence() const { return *m_sence; }
	private:
		Sence::SenceParse m_senceParse;
		std::unique_ptr<Sence::Sence> m_sence;
	};
}