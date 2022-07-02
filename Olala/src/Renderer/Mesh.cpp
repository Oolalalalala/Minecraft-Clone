#include "pch.h"
#include "Mesh.h"


namespace Olala {

	Olala::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		m_VertexBuffer = VertexBuffer::Create((float*)vertices.data(), vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(indices.data(), indices.size());
	}

	Olala::Mesh::~Mesh()
	{
	}

	Ref<Mesh> Olala::Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		return CreateRef<Mesh>(vertices, indices);
	}

}