#pragma once


namespace Olala {

	class ComputeShader
	{
	public:
		ComputeShader(const std::filesystem::path& filepath);
		~ComputeShader();

		void Bind() const;
		void Unbind() const;

		void Dispatch(uint32_t x, uint32_t y, uint32_t z);

		static Ref<ComputeShader> Create(const std::filesystem::path& filepath);

	private:
		std::string ReadFile(const std::filesystem::path& path);

	private:
		uint32_t m_RendererID = 0;
	};

}