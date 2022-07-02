#include "pch.h"
#include "VertexBuffer.h"
#include "VertexArray.h"

namespace Olala {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		return CreateRef<VertexBuffer>(size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		return CreateRef<VertexBuffer>(vertices, size);
	}

	VertexBuffer::VertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::Bind() const
	{
		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBindVertexBuffer(0, m_RendererID, 0, m_VertexArray->GetLayout().GetStride());
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

}