#include "pch.h"
#include "Framebuffer.h"
#include "Core/IO.h"

namespace Olala {

	static GLenum ToOpenGLType(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::None:
		case FramebufferTextureFormat::RGBA8:           return GL_RGBA8;
		case FramebufferTextureFormat::RGBA16F:         return GL_RGBA16F;
		case FramebufferTextureFormat::RED_INTEGER:     return GL_RED_INTEGER;
		case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
		}
	}

	static GLenum ToOpenGLType(FramebufferTextureFilterFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFilterFormat::None:
		case FramebufferTextureFilterFormat::Linear: return GL_LINEAR;
		case FramebufferTextureFilterFormat::Nearest: return GL_NEAREST;
		}
	}

	static GLenum ToOpenGLType(FramebufferTextureWrapFormat format)
	{
		switch (format)
		{
		case FramebufferTextureWrapFormat::None:
		case FramebufferTextureWrapFormat::ClampToEdge: return GL_CLAMP_TO_EDGE;
		case FramebufferTextureWrapFormat::ClampToBorder: return GL_CLAMP_TO_BORDER;
		case FramebufferTextureWrapFormat::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case FramebufferTextureWrapFormat::Repeat: return GL_REPEAT;
		}
	}

	static void AttachColorTexture(uint32_t rendererID, GLenum internalFormat, uint32_t width, uint32_t height,
		GLenum filter, GLenum wrap, uint32_t index)
	{
		glTextureStorage2D(rendererID, 1, internalFormat, width, height);

		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, filter);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, wrap);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, wrap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, rendererID, 0);
	}

	static void AttachDepthTexture(uint32_t rendererID, GLenum internalFormat, uint32_t width, uint32_t height, GLenum filter, GLenum wrap)
	{
		glTextureStorage2D(rendererID, 1, internalFormat, width, height);

		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, filter);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, wrap);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, wrap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rendererID, 0);
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specs)
	{
		return CreateRef<Framebuffer>(specs);
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specs)
		: m_Specification(specs)
	{
		for (auto& spec : m_Specification.Attachments)
		{
			if (spec.TextureFormat == FramebufferTextureFormat::DEPTH24STENCIL8)
				m_DepthAttachmentSpecification = spec;
			else
				m_ColorAttachmentSpecifications.emplace_back(spec);
		}

		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void Framebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			glCreateTextures(GL_TEXTURE_2D, m_ColorAttachments.size(), m_ColorAttachments.data());
			for (int i = 0; i < m_ColorAttachments.size(); i++)
			{
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					AttachColorTexture(m_ColorAttachments[i], GL_RGBA8, m_Specification.Width, m_Specification.Height,
						ToOpenGLType(m_ColorAttachmentSpecifications[i].FilterFormat),
						ToOpenGLType(m_ColorAttachmentSpecifications[i].WrapFormat), i);
					break;
				case FramebufferTextureFormat::RGBA16F:
					AttachColorTexture(m_ColorAttachments[i], GL_RGBA16F, m_Specification.Width, m_Specification.Height,
						ToOpenGLType(m_ColorAttachmentSpecifications[i].FilterFormat),
						ToOpenGLType(m_ColorAttachmentSpecifications[i].WrapFormat), i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					AttachColorTexture(m_ColorAttachments[i], GL_R32I, m_Specification.Width, m_Specification.Height,
						ToOpenGLType(m_ColorAttachmentSpecifications[i].FilterFormat),
						ToOpenGLType(m_ColorAttachmentSpecifications[i].WrapFormat), i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
			AttachDepthTexture(m_DepthAttachment, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height,
				ToOpenGLType(m_DepthAttachmentSpecification.FilterFormat),
				ToOpenGLType(m_DepthAttachmentSpecification.WrapFormat));
		}

		if (m_ColorAttachments.size() > 1)
		{
			GLenum* attachments = new GLenum[m_ColorAttachments.size()];
			for (int i = 0; i < m_ColorAttachments.size(); i++)
				attachments[i] = GL_COLOR_ATTACHMENT0 + i;

			glDrawBuffers(m_ColorAttachments.size(), attachments);
			delete[] attachments;
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			OLA_CORE_ERROR("Framebuffer creation incomplete, code : {0}", glCheckFramebufferStatus(GL_FRAMEBUFFER));


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	void Framebuffer::SetDrawBuffer(uint32_t idx)
	{
		GLenum buffer = GL_COLOR_ATTACHMENT0 + idx;
		glDrawBuffers(1, &buffer);
	}

	void Framebuffer::SetDrawBuffers(const std::vector<uint32_t>& indices)
	{
		std::vector<GLenum> buffers;
		buffers.resize(indices.size());
		for (int i = 0; i < indices.size(); i++)
			buffers[i] = GL_COLOR_ATTACHMENT0 + indices[i];

		glDrawBuffers(buffers.size(), buffers.data());
	}

	void Framebuffer::BindColorAttachment(uint32_t idx, uint32_t slot)
	{
		glBindTextureUnit(slot, m_ColorAttachments[idx]);
	}

	void Framebuffer::BindColorAttachment(uint32_t idx, uint32_t slot, GLenum usage)
	{
		glBindImageTexture(slot, m_ColorAttachments[idx], 0, false, 0, usage, ToOpenGLType(m_ColorAttachmentSpecifications[idx].TextureFormat));
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}