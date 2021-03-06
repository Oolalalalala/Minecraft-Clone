#pragma once

namespace Olala {

	class Texture2D
	{
	public:
		Texture2D(uint32_t width, uint32_t height, GLenum format, uint32_t levels = 1); //  TODO : use custom enum
		Texture2D(const std::string& path, bool sRGB = true); // TODO : add mipmap function
		~Texture2D();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		void Bind(uint32_t slot) const; // for sampler2d
		void Bind(uint32_t slot, uint32_t level, GLenum usage) const; // for image2d // TODO : use custom enum

		bool IsLoaded() const { return m_IsLoaded; }
		uint32_t GetRendererID() const { return m_RendererID; }

		bool operator==(const Texture2D& other)
		{
			return m_RendererID == other.m_RendererID;
		}

		static void LoadToGLFWImage(GLFWimage& image, const std::string& path);
		static void FreeGLFWImage(GLFWimage& image);


		static Ref<Texture2D> Create(const std::string& filepath);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, GLenum format, uint32_t levels = 1);

	private:
		bool m_IsLoaded = false;
		void* m_Data = nullptr;
		uint32_t m_Width = 0, m_Height = 0;
		uint32_t m_RendererID = 0;
		GLenum m_Format;
	};

}