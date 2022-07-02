#include "pch.h"
#include "Texture2D.h"

#include <stb_image/stb_image.h>

namespace Olala {

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		return CreateRef<Texture2D>(path);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, GLenum format, uint32_t levels)
	{
		return CreateRef<Texture2D>(width, height, format, levels);
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, GLenum format, uint32_t levels)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, levels, format, width, height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_Width = width;
		m_Height = height;
		m_Format = format;
	}

	Texture2D::Texture2D(const std::string& path, bool sRGB)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		auto* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			OLA_CORE_WARN("Cannot load texture 2D, path : {}", path);
			return;
		}

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
			dataFormat = GL_RGBA;
			m_Format = GL_RGBA8;
		}
		else if (channels == 3)
		{
			internalFormat = sRGB ? GL_SRGB8 : GL_RGB8;
			dataFormat = GL_RGB;
			m_Format = GL_RGB8;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		m_IsLoaded = true;
		m_Width = width, m_Height = height;

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::LoadToGLFWImage(GLFWimage& image, const std::string& path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);

		auto* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			OLA_CORE_WARN("Cannot load texture 2D, path : {}", path);
			return;
		}

		if (channels != 4)
		{
			OLA_CORE_ERROR("Texture format incorrect, path : {}", path);
			return;
		}

		image.width = width;
		image.height = height;
		image.pixels = data;
	}

	void Texture2D::FreeGLFWImage(GLFWimage& image)
	{
		stbi_image_free(image.pixels);
	}

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void Texture2D::Bind(uint32_t slot, uint32_t level, GLenum usage) const
	{
		glBindImageTexture(slot, m_RendererID, level, false, 0, usage, m_Format);
	}

}