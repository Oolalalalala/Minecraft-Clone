#include "Core/Base.h"
#include "../Shader.h"
#include "../RenderTarget.h"
#include "../ComputeShader.h"

namespace Olala {

	class ToneMapping
	{
	public:
		static void Init();
		static void Apply(Ref<RenderTarget> target);

	private:
		static Ref<ComputeShader> s_ACESShader;
	};

}