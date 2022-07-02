#pragma once

#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Joint.h"
#include "Animation.h"
#include "RiggedMesh.h"


namespace Olala {

	class Model
	{
	public:
		Model(Ref<Mesh> mesh, Ref<Material> material)
			: m_Mesh(mesh), m_Material(material) {}

		Ref<Mesh> GetMesh() { return m_Mesh; }
		Ref<Material> GetMaterial() { return m_Material; }

		static Ref<Model> Create(Ref<Mesh> mesh, Ref<Material> material) { return CreateRef<Model>(mesh, material); }

	private:
		Ref<Mesh> m_Mesh;
		Ref<Material> m_Material;

		friend class ColladaParser;
	};


	class RiggedModel
	{
	public:
		RiggedModel(Ref<RiggedMesh> mesh, Ref<Material> material)
			: m_Mesh(mesh), m_Material(material) {}

		Ref<RiggedMesh> GetMesh() { return m_Mesh; }
		Ref<Material> GetMaterial() { return m_Material; }

	private:
		Ref<RiggedMesh> m_Mesh;
		Ref<Material> m_Material;
		std::vector<Joint> m_Joints;
		uint32_t m_RootJointID;

		std::unordered_map<std::string, Ref<Animation>> m_Animations;

		friend class ColladaParser;
	};

}