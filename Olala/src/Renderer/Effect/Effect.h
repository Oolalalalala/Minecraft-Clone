#pragma once

#include "Core/Base.h"
#include "../RenderTarget.h"


namespace Olala {

	class Effect
	{
	public:
		static void Init();
		static void Bloom(Ref<RenderTarget> target);
		static void ToneMapping(Ref<RenderTarget> target);
	};

}