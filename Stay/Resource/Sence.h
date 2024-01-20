#pragma once

#include <vector>

#include "SenceObject.h"

namespace stay::Sence
{
	class Sence
	{
	public:
		std::vector<std::unique_ptr<Mesh>> m_meshs;
	};
}
