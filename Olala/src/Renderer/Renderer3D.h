#pragma once

#include "glm/glm.hpp"

#include "Core/Base.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "CubeMap.h"


namespace Olala {

	class Renderer3D
	{
	public:
		static void Init();
		static void ShutDown();

		static void BeginScene(Ref<Camera> camera);
		static void EndScene();

		static void Submit(Ref<Mesh> mesh, Ref<Material> material, glm::mat4 modelMatrix);
		static void AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, const float& intensity, bool castShadow);
		static void AddPointLight(const glm::vec3& position, const glm::vec3& color, const float& range, const float& intensity, bool castShadow);
		static void AddSpotLight(const glm::vec3& position, const glm::vec3 direction, const glm::vec3& color, const float& angle, const float& range, const float& intensity, bool castShadow);
		static void SetSkyBox(Ref<CubeMap> skyBox, const glm::vec4& color);

	private:
		static void Flush();
		static void Reset();

		struct DirectionalLightUniformData
		{
			glm::vec4 Direction; // With a [castShadow] flag in w component
			glm::vec4 Color; // w Component not used
		};

		static DirectionalLightUniformData s_DirectionalLightUniformData;
	};

}