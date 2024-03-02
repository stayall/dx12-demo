#pragma once

#include <list>
#include <string>

#include "SenceObject.h"

namespace stay::Sence
{
	class BaseSenceNode
	{
	public:
		BaseSenceNode(const std::string& name) : m_name(name) {}

		void AddChild(std::shared_ptr<BaseSenceNode>&& node) { m_nodes.push_back(std::move(node)); }
		void AddChild(std::shared_ptr<Transform>&& node) { m_transform.push_back(std::move(node)); }
		virtual ~BaseSenceNode() = default;

	private:
		std::string m_name;

		std::list<std::shared_ptr<BaseSenceNode>> m_nodes;
		std::list<std::shared_ptr<Transform>> m_transform;
	};

	using EmptyNode = BaseSenceNode;

	template<class T>
	class SenceNode : public BaseSenceNode
	{
	public:
		SenceNode(std::shared_ptr<T>& object) : BaseSenceNode("") { m_currentObject = object; };
		SenceNode(std::shared_ptr<T>&& object) : BaseSenceNode("") { m_currentObject = std::move(object); };
	private:
		std::shared_ptr<T> m_currentObject;
	};

	class GeometryNode : public SenceNode<Geometry>
	{
	public:
		GeometryNode(std::shared_ptr<Geometry>& object) : SenceNode<Geometry>(object) {};
		GeometryNode(std::shared_ptr<Geometry>&& object) : SenceNode<Geometry>(object) {};
	private:
		bool m_castShadow = false;
		bool m_visible = true;
	};

	class LightNode : public SenceNode<Light>
	{
	public:
		LightNode(std::shared_ptr<Light>& object) : SenceNode<Light>(object) {};
		LightNode(std::shared_ptr<Light>&& object) : SenceNode<Light>(object) {};
	private:

	};

	class CameraNode : public SenceNode<Camera>
	{
	public:
		CameraNode(std::shared_ptr<Camera>& object) : SenceNode<Camera>(object) {};
		CameraNode(std::shared_ptr<Camera>&& object) : SenceNode<Camera>(object) {};
		void SetTarget(const Math::Vector& target) { m_target = target; }
	private:
		Math::Vector m_target;
	};

}