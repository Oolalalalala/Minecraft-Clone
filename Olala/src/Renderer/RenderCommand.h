#pragma once

#include "VertexArray.h"
#include "RenderTarget.h"
#include "glm/glm.hpp"

namespace Olala {

	enum class BlendFactor
	{
		ZERO = 0, ONE, SRC_ALPHA, ONE_MINUS_SRC_ALPHA
	};

	class RenderCommand
	{
	public:
		static void Init();

		static void SetClearColor(const glm::vec4 color);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void UseDepthTest(bool enabled);
		static void UseBlend(bool enabled);
		static void SetBlendMethod(BlendFactor src = BlendFactor::SRC_ALPHA, BlendFactor dst = BlendFactor::ONE_MINUS_SRC_ALPHA);

		static void SetRenderTarget(Ref<RenderTarget> renderTarget);
		static void SetRenderTargetDefaultFramebuffer();

		static void DrawFillingQuad();

		static void Clear();
		static void DrawIndexed(Ref<VertexArray> vertexArray);

	private:
		static Ref<VertexArray> s_FillingQuadVertexArray;
	};

}