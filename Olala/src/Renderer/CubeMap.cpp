#include "pch.h"
#include "CubeMap.h"
#include "stb_image/stb_image.h"

namespace Olala {

	Ref<CubeMap> CubeMap::Create(const std::string& folder, const std::unordered_map<Face, std::string>& faces)
	{
		return CreateRef<CubeMap>(folder, faces);
	}

	CubeMap::CubeMap(const std::string& folder, const std::unordered_map<Face, std::string>& faces)
	{
		// Note : all faces of a cubemap should have the same data format
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, channels;
		GLenum internalFormat = 0, dataFormat = 0;

		stbi_set_flip_vertically_on_load(0);
		std::string defaultFaceNames[6] = { "right.png", "left.png", "top.png", "bottom.png", "back.png", "front.png" };

		for (int i = 0; i < 6; i++)
		{
			std::string faceName = folder + "/" + (faces.find((Face)i) == faces.end() ? defaultFaceNames[i] : faces.find((Face)i)->second);

			auto* data = stbi_load(faceName.c_str(), &width, &height, &channels, 0);
			if (!data)
			{
				OLA_CORE_WARN("Failed to load cube map, path : {}", folder);
				return;
			}
			if (channels == 4)
			{
				internalFormat = GL_SRGB8_ALPHA8;
				//internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_SRGB8;
				//internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	CubeMap::~CubeMap()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void CubeMap::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_RendererID);
	}

}