#include "pch.h"
#include "Bloom.h"

#include "../RenderCommand.h"
#include "../ComputeShader.h"
#include "../Texture2D.h"
#include "Core/IO.h"

namespace Olala {

	Ref<ComputeShader> Bloom::s_PrefilterShader;
	Ref<ComputeShader> Bloom::s_DownSampleShader;
	Ref<ComputeShader> Bloom::s_UpSampleShader;
	Ref<Shader> Bloom::s_CopyShader;
	std::vector<Ref<Texture2D>> Bloom::s_MipTextures;
	float Bloom::s_Threshold = 1.0f;
	uint32_t Bloom::s_Level = 5;
	float Bloom::s_AspectRatio = 16.f / 9.f;
	std::vector<glm::ivec2> Bloom::s_MipSizes;


	void Bloom::Init()
	{
		s_PrefilterShader = ComputeShader::Create("../Olala/Asset/Internal/Shader/Prefilter.compute");
		s_DownSampleShader = ComputeShader::Create("../Olala/Asset/Internal/Shader/DownSample.compute");
		s_UpSampleShader = ComputeShader::Create("../Olala/Asset/Internal/Shader/UpSample.compute");
		s_CopyShader = Shader::Create("../Olala/Asset/Internal/Shader/Display.glsl");

		CreateMipTexture(1920, 1080);
	}

	void Bloom::Apply(Ref<RenderTarget> target)
	{
		auto targetFramebuffer = target->GetFramebuffer();
		auto& targetSpecs = targetFramebuffer->GetSpecification();
		float aspectRatio = (float)targetSpecs.Width / targetSpecs.Height;
		if (s_AspectRatio != aspectRatio)
		{
			s_AspectRatio = aspectRatio;
			CreateMipTexture(1080 * aspectRatio, 1080);
		}
		
		s_PrefilterShader->Bind();
		targetFramebuffer->BindColorAttachment(0, 0, GL_READ_WRITE);
		s_MipTextures[0]->Bind(1, 0, GL_READ_WRITE);
		s_PrefilterShader->Dispatch((s_MipSizes[0].x / 8) + 1, (s_MipSizes[0].y / 8) + 1, 1);


		s_DownSampleShader->Bind();
		for (int i = 0; i < s_Level; i++)
		{
			s_MipTextures[i]->Bind(0, 0, GL_READ_ONLY);
			s_MipTextures[i + 1]->Bind(1, 0, GL_WRITE_ONLY);
			s_DownSampleShader->Dispatch((s_MipSizes[i + 1].x / 8) + 1, (s_MipSizes[i + 1].y / 8) + 1, 1);
		}
		
		s_UpSampleShader->Bind();
		for (int i = s_Level; i > 0; i--)
		{
			s_MipTextures[i]->Bind(0, 0, GL_READ_ONLY);
			s_MipTextures[i - 1]->Bind(1, 0, GL_READ_WRITE);
			s_DownSampleShader->Dispatch((s_MipSizes[i - 1].x / 8) + 1, (s_MipSizes[i - 1].y / 8) + 1, 1);
		}

		targetFramebuffer->Bind();
		targetFramebuffer->SetDrawBuffer(0);
		s_CopyShader->Bind();
		s_MipTextures[0]->Bind(0);
		s_CopyShader->SetUniformInt("u_Texture", 0);
		RenderCommand::SetBlendMethod(BlendFactor::ONE, BlendFactor::ONE);
		RenderCommand::DrawFillingQuad();
		RenderCommand::SetBlendMethod();
	}

	void Bloom::CreateMipTexture(uint32_t width, uint32_t height)
	{
		s_MipTextures.resize(s_Level + 1);
		s_MipSizes.resize(s_Level + 1);
		uint32_t w = width, h = height;
		for (int i = 0; i <= s_Level; i++)
		{
			s_MipTextures[i] = Texture2D::Create(w, h, GL_RGBA16F);
			s_MipSizes[i] = glm::ivec2(w, h);
			w = std::max(1U, w / 2);
			h = std::max(1U, h / 2);
		}
	}

}