#pragma once

#include "Model.h"

namespace Olala {

	class ColladaParser
	{
	public:
		static Ref<Model> Parse(const std::filesystem::path& path);
	};

}