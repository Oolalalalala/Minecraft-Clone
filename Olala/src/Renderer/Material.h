#pragma once

#include "Core/Base.h"
#include "Texture2D.h"
#include "glm/glm.hpp"


namespace Olala {

	class Material
	{
	public:
		Material() {}
		Material(Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f), float metallic = 0.f, float roughness = 0.f)
			: Texture(texture), Color(color), Metallic(metallic), Roughness(roughness) {}
		~Material() {}

		static Ref<Material> Create(Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f), float metallic = 0.f, float roughness = 0.f)
		{
			return CreateRef<Material>(texture, color, metallic, roughness);
		}

	public:
		Ref<Texture2D> Texture;
		glm::vec4 Color = glm::vec4(1.0f);
		float Metallic = 0.f;
		float Roughness = 0.f;
	};
}