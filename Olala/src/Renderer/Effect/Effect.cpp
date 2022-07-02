#include "pch.h"
#include "Effect.h"

#include "Bloom.h"
#include "ToneMapping.h"
#include "Core/IO.h"

namespace Olala {

	void Effect::Init()
	{
		Bloom::Init();
		ToneMapping::Init();
	}

	void Effect::Bloom(Ref<RenderTarget> target)
	{
		if (IO::IsKeyPressed(GLFW_KEY_X)) return;
		Bloom::Apply(target);
	}

	void Effect::ToneMapping(Ref<RenderTarget> target)
	{
		ToneMapping::Apply(target);
	}

}