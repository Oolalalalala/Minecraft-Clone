#include "pch.h"
#include "RenderCommand.h"

namespace Olala {

	Ref<VertexArray> RenderCommand::s_FillingQuadVertexArray;

	static GLenum ToOpenGLType(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::ZERO: return GL_ZERO;
		case BlendFactor::ONE: return GL_ONE;
		case BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
		case BlendFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		}

		OLA_CORE_ERROR("Unknown blend factor");
		return GL_NONE;
	}

	void RenderCommand::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);


		const float vertices[] = { -1.0, -1.0,  0.0,    0.0, 0.0,
								    1.0, -1.0,  0.0,    1.0, 0.0,
								    1.0,  1.0,  0.0,    1.0, 1.0,
								   -1.0,  1.0,  0.0,    0.0, 1.0 };
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		s_FillingQuadVertexArray = VertexArray::Create();
		s_FillingQuadVertexArray->SetLayout({ { "a_Position", ShaderDataType::Float3},
								              { "a_TexCoord", ShaderDataType::Float2} });
		s_FillingQuadVertexArray->AddBuffer(VertexBuffer::Create(vertices, sizeof(vertices)), IndexBuffer::Create(indices, 6));
	}

	void RenderCommand::SetClearColor(const glm::vec4 color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::UseDepthTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void RenderCommand::UseBlend(bool enabled)
	{
		if (enabled)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	void RenderCommand::SetBlendMethod(BlendFactor src, BlendFactor dst)
	{
		glBlendFunc(ToOpenGLType(src), ToOpenGLType(dst));
	}

	void RenderCommand::SetRenderTarget(Ref<RenderTarget> renderTarget)
	{
		renderTarget->GetFramebuffer()->Bind();
	}

	void RenderCommand::SetRenderTargetDefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderCommand::DrawFillingQuad()
	{
		DrawIndexed(s_FillingQuadVertexArray);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::DrawIndexed(Ref<VertexArray> vertexArray)
	{
		vertexArray->Bind();
		for (auto [vertexBuffer, indexBuffer] : vertexArray->GetBuffers())
		{
			indexBuffer->Bind();
			vertexBuffer->Bind();
			glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		}
		vertexArray->Unbind();
	}

}