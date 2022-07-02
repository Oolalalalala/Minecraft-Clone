#pragma once

#include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Core/Base.h"

namespace Olala {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void SetLayout(const VertexBufferLayout& layout);
		const VertexBufferLayout& GetLayout() const { return m_Layout; }

		void AddBuffer(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer);
		void ClearBuffers();

		const std::vector<std::pair<Ref<VertexBuffer>, Ref<IndexBuffer>>>& GetBuffers() const { return m_Buffers; }

		static Ref<VertexArray> Create();

	private:
		GLuint m_RendererID;
		VertexBufferLayout m_Layout;
		std::vector<std::pair<Ref<VertexBuffer>, Ref<IndexBuffer>>> m_Buffers;
	};

}