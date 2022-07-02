#include "pch.h"
#include "ToneMapping.h"
#include "../RenderCommand.h"
#include "Core/IO.h"


namespace Olala {

	Ref<ComputeShader> ToneMapping::s_ACESShader;

	void ToneMapping::Init()
	{
		s_ACESShader = ComputeShader::Create("../Olala/Asset/Internal/Shader/ToneMap.compute");
	}

	void ToneMapping::Apply(Ref<RenderTarget> target)
	{
		auto targetFramebuffer = target->GetFramebuffer();
		auto& targetSpecs = targetFramebuffer->GetSpecification();

		targetFramebuffer->BindColorAttachment(0, 0, GL_READ_WRITE);
		s_ACESShader->Bind();
		s_ACESShader->Dispatch(targetSpecs.Width / 8 + 1, targetSpecs.Height / 8 + 1, 1);
	}

}