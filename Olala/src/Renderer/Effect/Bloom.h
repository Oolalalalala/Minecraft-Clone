#pragma once

#include "Core/Base.h"
#include "../RenderTarget.h"
#include "../Shader.h"
#include "../ComputeShader.h"
#include "../Texture2D.h"

namespace Olala {


	class Bloom
	{
	public:
		static void Init();
		static void Apply(Ref<RenderTarget> target);

	private:
		static void CreateMipTexture(uint32_t width, uint32_t height);

	private:
		static Ref<ComputeShader> s_PrefilterShader;
		static Ref<ComputeShader> s_DownSampleShader;
		static Ref<ComputeShader> s_UpSampleShader;
		static Ref<Shader> s_CopyShader;
		static std::vector<Ref<Texture2D>> s_MipTextures;
		static float s_Threshold;
		static uint32_t s_Level;
		static float s_AspectRatio;
		static std::vector<glm::ivec2> s_MipSizes;
	};

}