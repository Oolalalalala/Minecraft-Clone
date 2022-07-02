#pragma once

#include "Core/Base.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"


namespace Olala {

	struct WeightedVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 JointIDs;
		glm::vec3 Weights;
	};


	class RiggedMesh
	{
	public:
		RiggedMesh(const std::vector<WeightedVertex>& vertices, const std::vector<uint32_t>& indices);
		~RiggedMesh();


		static Ref<RiggedMesh> Create(const std::vector<WeightedVertex>& vertices, const std::vector<uint32_t>& indices);

	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}