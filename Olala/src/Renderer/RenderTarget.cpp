#include "pch.h"
#include "RenderTarget.h"

#include "Core/IO.h"


namespace Olala {

	Ref<RenderTarget> RenderTarget::Create(RenderMode mode)
	{
		return CreateRef<RenderTarget>(mode);
	}

	RenderTarget::RenderTarget(RenderMode mode)
	{
		FramebufferSpecification specs;
		auto [width, height] = IO::GetWindowSize();
		specs.Width = width, specs.Height = height;

		if (mode == RenderMode::Forward)
		{
			specs.Attachments = {
				{ FramebufferTextureFormat::RGBA16F, FramebufferTextureFilterFormat::Linear, FramebufferTextureWrapFormat::Repeat },
				{ FramebufferTextureFormat::DEPTH24STENCIL8, FramebufferTextureFilterFormat::Linear, FramebufferTextureWrapFormat::Repeat }
			};
		}
		else if (mode == RenderMode::Deferred)
		{
			specs.Attachments = {
				{ FramebufferTextureFormat::RGBA16F, FramebufferTextureFilterFormat::Nearest, FramebufferTextureWrapFormat::Repeat }, // The output buffer
				{ FramebufferTextureFormat::RGBA16F, FramebufferTextureFilterFormat::Nearest, FramebufferTextureWrapFormat::Repeat }, // Position buffer
				{ FramebufferTextureFormat::RGBA16F, FramebufferTextureFilterFormat::Nearest, FramebufferTextureWrapFormat::Repeat }, // Normal buffer
				{ FramebufferTextureFormat::RGBA16F, FramebufferTextureFilterFormat::Nearest, FramebufferTextureWrapFormat::Repeat }, // Albedo buffer
				{ FramebufferTextureFormat::DEPTH24STENCIL8, FramebufferTextureFilterFormat::Linear, FramebufferTextureWrapFormat::Repeat }
			};
		}

		m_Framebuffer = Framebuffer::Create(specs);
	}

}