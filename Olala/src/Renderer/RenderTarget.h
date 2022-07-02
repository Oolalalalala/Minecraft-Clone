#pragma once

#include "Core/Base.h"
#include "Framebuffer.h"

namespace Olala {

	enum class RenderMode
	{
		Forward = 0, Deferred
	};

	// Wrapper class for framebuffer
	class RenderTarget
	{
	public:
		RenderTarget(RenderMode mode);

		inline void Resize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

		inline RenderMode GetRenderMode() { return m_RenderMode; }
		inline Ref<Framebuffer> GetFramebuffer() { return m_Framebuffer; }

		static Ref<RenderTarget> Create(RenderMode mode);

	private:
		RenderMode m_RenderMode;
		Ref<Framebuffer> m_Framebuffer;
	};

}