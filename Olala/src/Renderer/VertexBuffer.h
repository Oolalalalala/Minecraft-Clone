#pragma once

#include "VertexBufferLayout.h"

namespace Olala {

	class VertexArray;

	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size);
		VertexBuffer(const float* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size);


		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(const float* vertices, uint32_t size);

	private:
		uint32_t m_RendererID;
		VertexArray* m_VertexArray = nullptr;

		friend class VertexArray;
	};

}