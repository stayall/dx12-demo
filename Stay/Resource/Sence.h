#pragma once

#include <vector>

#include "SenceObject.h"
#include "SenceNode.h"

namespace stay::Sence
{
	class Sence
	{

	public:
		Sence() { m_senceNode = std::make_shared<EmptyNode>("Root"); }

		std::shared_ptr<BaseSenceNode> m_senceNode;

		std::vector<std::shared_ptr<Geometry>> m_geometries;
		std::vector<std::shared_ptr<Material>> m_materials;
		std::vector<std::shared_ptr<Light>> m_lights;
		std::vector<std::shared_ptr<Camera>> m_cameras;
	};
}
