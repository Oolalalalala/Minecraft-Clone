#pragma once

#include "Core/Base.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


namespace Olala {

	struct Vertex
	{
		glm::vec3 Position = glm::vec3(0.0);
		glm::vec3 Normal = glm::vec3(0.0);
		glm::vec2 TexCoord = glm::vec3(0.0);

		Vertex() {};
		Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord)
			: Position(position), Normal(normal), TexCoord(texCoord) {}
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

		static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}