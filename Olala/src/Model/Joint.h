#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Olala {

	struct Joint
	{
		std::vector<uint32_t> Children;
		glm::vec3 Position;
		glm::quat Rotation;
	};

}