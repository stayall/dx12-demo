#pragma once

#include <memory>
#include <vector>

#include <crossguid/guid.hpp>

#include <Core/Math/MathType.h>
#include "Platform/Windows/RHI/VertexData.h"


#include "Image.h"

namespace stay::Sence
{

	enum class SenceObjectType
	{
		Invaild = -1,
		kGeometry,
		kMesh,
		kMeshVertex,
		kMeshIndex,
		kTransform,
		kMaterial,
		kTexture,
		kCamera,
		kLight
	};

	class BaseSenceObject
	{
	protected:
		BaseSenceObject(SenceObjectType type) : m_type(type) { m_guid = xg::newGuid(); }
		BaseSenceObject(const BaseSenceObject& rhs) = delete;
		BaseSenceObject& operator=(const BaseSenceObject& rhs) = delete;

		BaseSenceObject(BaseSenceObject&& rhs) = default;
		BaseSenceObject& operator=(BaseSenceObject&& rhs) = default;
	protected:
		SenceObjectType m_type;
		xg::Guid m_guid;
	};


	class VertexArray : public BaseSenceObject
	{
	public:
		

		VertexArray(VertexDataType type, Math::Float* srcData, size_t count);
		VertexArray(VertexDataType type, std::shared_ptr<Math::Float[]>& srcData, size_t count);

		size_t GetVertexCount() const { return m_size; }
		VertexDataType GetVertexDataType() const { return m_type; }

		Math::Float* GetVertex() const { return m_data.get(); }
	private:
		VertexDataType m_type;
		size_t m_size;

		std::shared_ptr<Math::Float[]> m_data;
	};

	class IndexArray : public BaseSenceObject
	{
	public:

		IndexArray(uint32_t* srcData, size_t count, uint32_t materialIndex);
		IndexArray(const std::vector<uint32_t>& srcData, size_t count, uint32_t materialIndex);
		IndexArray(std::shared_ptr<uint32_t[]>& srcData, size_t count);

		size_t GetIndexCount() const { return m_size; }
		uint32_t* GetVertexIndex() const { return m_data.get(); }
	private:
		uint32_t m_materialIndex;
		uint32_t m_restartIndex;

		size_t m_size;
		std::shared_ptr<uint32_t[]> m_data;
	};

	class Mesh : public BaseSenceObject
	{
	public:
		Mesh();

		void AddVertexData(VertexDataType type, Math::Float* srcData, size_t count) { m_numSubMesh++; m_vertexDatas[type].emplace_back(type, srcData, count); }
		void AddVertexData(VertexDataType type, std::shared_ptr<Math::Float[]>& srcData, size_t count) { m_numSubMesh++; m_vertexDatas[type].emplace_back(type, srcData, count); }

		void AddVertexIndex(uint32_t* srcData, size_t count, uint32_t materialIndex) { m_numSubMesh++; m_vertexIndice.emplace_back(srcData, count, materialIndex); }
		void AddVertexIndex(const std::vector<uint32_t>& srcData, size_t count, uint32_t materialIndex) { m_vertexIndice.emplace_back(srcData, count, materialIndex); }
		void AddVertexIndex(std::shared_ptr<uint32_t[]>& srcData, size_t count) { m_vertexIndice.emplace_back(srcData, count); }

		size_t GetMeshCount() const { return m_numSubMesh; }

		float* GetVertexData(size_t index, VertexDataType type = VertexDataType::kPosition) const;
		size_t GetVertexCount(size_t index) const;

		uint32_t* GetIndexData(size_t index) const;
		size_t GetIndexCount(size_t index) const;
	private:
		size_t m_numSubMesh = 0;
		std::vector<VertexArray> m_vertexDatas[VertexDataType::kNumVertexDataType];
		std::vector<IndexArray> m_vertexIndice;
	};

	class Transform : BaseSenceObject
	{
	public:
		Transform() : BaseSenceObject(SenceObjectType::kTransform) {};

	private:
		Math::Vector m_position;
		Math::Quaternion m_rotation;
		Math::Vector m_scale = {1.0f, 1.0f, 1.0f};

		Math::Matrix m_tranformMatrix;
	};

	class MeshNode
	{
	public:
		MeshNode(const std::string& name) : m_name(name) {}

		void AddChild(std::shared_ptr<MeshNode>&& node) { m_nodes.emplace_back(std::move(node)); }
		void AddChild(std::shared_ptr<Transform>&& node) { m_transform.emplace_back(std::move(node)); }
		virtual ~MeshNode() = default;

	private:
		std::string m_name;

		std::list<std::shared_ptr<MeshNode>> m_nodes;
		std::list<std::shared_ptr<Transform>> m_transform;
	};

	class Geometry : public BaseSenceObject
	{
	public:
		Geometry() : BaseSenceObject(SenceObjectType::kGeometry) {};
		void AddMesh(std::unique_ptr<Mesh>&& mesh) { m_meshs.emplace_back(std::move(mesh)); }

		//
		std::shared_ptr<MeshNode> GetRoot() const { return m_children; }
		void SetRoot(const std::shared_ptr<MeshNode> &root)  { m_children = root; }
	private:
		//ASSImp Support
		std::shared_ptr<MeshNode> m_children;

		std::vector<std::unique_ptr<Mesh>> m_meshs;
		Transform m_transform;

	};

	class Texture : public BaseSenceObject
	{
	public:
		Texture() : BaseSenceObject(SenceObjectType::kTexture) {}
		Texture(const std::string& name) : BaseSenceObject(SenceObjectType::kTexture), m_name(name) { LoadTexture(); }

		void SetName(const std::string& name) { m_name = name; }
		void LoadTexture();
		std::shared_ptr<Image> GetImage() const { return m_image; }
	private:
		std::string m_name;
		std::shared_ptr<Image> m_image = nullptr;

	};
	template <typename T>
	struct ParameterValueMap
	{
		void SetValue(const T& rhs)
		{
			bUsingSingleValue = true;
			Value = rhs;
		}
		T GetValue() const { return Value; };

		void SetTexture(const std::shared_ptr<Texture>& rhs)
		{
			bUsingSingleValue = false;
			Map = rhs;
		}

		std::shared_ptr<Texture> GetTexture() const { return  Map; }

	private:
		bool bUsingSingleValue = true;
		T Value;
		std::shared_ptr<Texture> Map;
	};

	class Material : BaseSenceObject
	{
	public:
		Material() : BaseSenceObject(SenceObjectType::kMaterial) {};
		~Material() {};


#define XX(attri, type) void Set##attri##Value(type value) {m_##attri.SetValue(value);}

		XX(BaseColor, Math::Color);
		XX(Normal, Math::Vector);
		XX(Specular, Math::Color);
		XX(Roughness, Math::Float);
		XX(Metallic, Math::Float);
		XX(AmbientOcclusion, Math::Float);

#undef XX

#define XX(attri) void Set##attri##Texture(std::shared_ptr<Texture> value) {m_##attri.SetTexture(value);}

		XX(BaseColor);
		XX(Normal);
		XX(Specular);
		XX(Roughness);
		XX(Metallic);
		XX(AmbientOcclusion);
#undef XX

#define XX(attri, type) \
	type Get##attri##Vaule() const {return m_##attri.GetValue();}

		XX(BaseColor, Math::Color);
		XX(Normal, Math::Vector);
		XX(Specular, Math::Color);
		XX(Roughness, Math::Float);
		XX(Metallic, Math::Float);
		XX(AmbientOcclusion, Math::Float);
#undef XX

#define XX(attri, type) \
	std::shared_ptr<Texture> Get##attri##Texure() const {return m_##attri.GetTexture();}

		XX(BaseColor, Math::Color);
		XX(Normal, Math::Vector);
		XX(Specular, Math::Color);
		XX(Roughness, Math::Float);
		XX(Metallic, Math::Float);
		XX(AmbientOcclusion, Math::Float);
#undef XX

	private:
		ParameterValueMap<Math::Color> m_BaseColor = {};
		ParameterValueMap<Math::Vector> m_Normal = {};
		ParameterValueMap<Math::Color> m_Specular = {};
		ParameterValueMap<Math::Float> m_Roughness = {};
		ParameterValueMap<Math::Float> m_Metallic = {};
		ParameterValueMap<Math::Float> m_AmbientOcclusion = {};
	};


	class Light : public BaseSenceObject
	{
	public:
		using AttenFunc = Math::Float(Math::Float, Math::Float);
		enum LightType
		{
			kInvaild,
			kDirectionalLight,
			kPointLight,

		};

		Light() : BaseSenceObject(SenceObjectType::kLight) {};
		Light(LightType type, Math::Vector color, Math::Float intensity = 1.0f) : BaseSenceObject(SenceObjectType::kLight), m_type(type), m_color(color), m_intensity(intensity) {}


	protected:
		LightType m_type = kInvaild;

		Math::Vector m_color;
		Math::Float m_intensity;

		Math::Float m_nearCilp = 0.0f;
		Math::Float m_farCilp = 1000.0f;

		AttenFunc m_lightAttenuation;
		bool m_castShadow;

	};

	class DirectionalLight : public Light
	{
	public:
		DirectionalLight() = default;
		DirectionalLight(Math::Vector color, Math::Vector direction, Math::Float intensity = 1.0f)
			: Light(kDirectionalLight, color, intensity), m_direction(direction) {};
	private:
		Math::Vector m_direction;
	};

	class Camera :public BaseSenceObject
	{
	public:
		enum CameraType
		{
			kProjection,
			kOrthogonal
		};
		Camera() : BaseSenceObject(SenceObjectType::kCamera) {};
		Camera(CameraType type, Math::Float aspectRatio, Math::Float nearClip = 0.01f, Math::Float farClip = 1000.0f)
			: BaseSenceObject(SenceObjectType::kCamera),
			m_type(type), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip) {}

	protected:
		CameraType m_type;

		Math::Float m_aspectRatio;

		Math::Float m_nearClip;
		Math::Float m_farClip;
	};

	class ProjectionCamera : public Camera
	{
	public:
		ProjectionCamera(Math::Float fov, Math::Float aspectRatio, Math::Float nearClip = 0.01f, Math::Float farClip = 1000.0f) :
			Camera(kProjection, aspectRatio, nearClip, farClip), m_fov(fov) {}

	private:
		Math::Float m_fov;
	};
}