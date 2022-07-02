#include "pch.h"
#include "ComputeShader.h"

namespace Olala {

	ComputeShader::ComputeShader(const std::filesystem::path& filepath)
	{
		std::string source = ReadFile(filepath);

		uint32_t compute = glCreateShader(GL_COMPUTE_SHADER);
		const char* src = source.c_str();
		glShaderSource(compute, 1, &src, nullptr);
		glCompileShader(compute);

		// Check for error
		GLint result;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &length);
			char* message = new char[length];
			glGetShaderInfoLog(compute, length, &length, message);
			OLA_CORE_WARN("Shader compilation failed, path : {0}\nMessage : \n{1}", filepath.string(), message);
			delete[] message;
		}

		// Create shader
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, compute);
		glLinkProgram(m_RendererID);
		glValidateProgram(m_RendererID);
		
		// Recycle
		glDetachShader(m_RendererID, compute);
		glDeleteShader(compute);
	}

	ComputeShader::~ComputeShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void ComputeShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void ComputeShader::Unbind() const
	{
		glUseProgram(0);
	}

	void ComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z)
	{
		glDispatchCompute(x, y, z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	Ref<ComputeShader> ComputeShader::Create(const std::filesystem::path& filepath)
	{
		return CreateRef<ComputeShader>(filepath);
	}

	std::string ComputeShader::ReadFile(const std::filesystem::path& path)
	{
		std::string result;
		std::ifstream in(path);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = (size_t)in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
				OLA_CORE_WARN("Cannot open shader file, path : {0}", path.string());
		}
		else
			OLA_CORE_WARN("Cannot open shader file, path : {0}", path.string());
		
		return result;
	}

}