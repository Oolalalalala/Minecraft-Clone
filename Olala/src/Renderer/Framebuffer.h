#pragma once

namespace Olala {


	enum class FramebufferTextureFormat
	{
		None = 0, RGBA8, RGBA16F, RED_INTEGER, DEPTH24STENCIL8
	};

	enum class FramebufferTextureFilterFormat
	{
		None = 0, Nearest, Linear
	};

	enum class FramebufferTextureWrapFormat
	{
		None = 0, Repeat, MirroredRepeat, ClampToEdge, ClampToBorder
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat textureFormat,
			FramebufferTextureFilterFormat filterFormat, FramebufferTextureWrapFormat wrapFormat)
			: TextureFormat(textureFormat), WrapFormat(wrapFormat), FilterFormat(filterFormat) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		FramebufferTextureFilterFormat FilterFormat = FramebufferTextureFilterFormat::None;
		FramebufferTextureWrapFormat WrapFormat = FramebufferTextureWrapFormat::None;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		std::vector<FramebufferTextureSpecification> Attachments;
	};


	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& specs);
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void Invalidate();
		void Resize(uint32_t width, uint32_t height);
		void SetDrawBuffer(uint32_t idx);
		void SetDrawBuffers(const std::vector<uint32_t>& indices);

		void BindColorAttachment(uint32_t idx = 0, uint32_t slot = 0);
		void BindColorAttachment(uint32_t idx, uint32_t slot, GLenum usage);

		const FramebufferSpecification& GetSpecification() { return m_Specification; }
		uint32_t GetColorAttachmentRendererID(uint32_t idx = 0) const { return m_ColorAttachments[idx]; }


		static Ref<Framebuffer> Create(const FramebufferSpecification& specs);

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}