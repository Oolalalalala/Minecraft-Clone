#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture2D.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/CubeMap.h"

#include <string>
#include <map>
#include <typeindex>


namespace Olala {

#define NOT_USER_DEFINED_SCRIPT constexpr static bool IsScript() { return false; }

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		entt::entity Parent = entt::null;
		std::vector<entt::entity> Children;

		TransformComponent() = default;
		TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
			: Position(position), Rotation(rotation), Scale(scale)
		{
		}

		glm::mat4 GetTransform()
		{
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(glm::quat(glm::radians(Rotation)))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct CameraComponent
	{
		Ref<Camera> Camera = nullptr;
		bool IsMain = false, IsOn = true;
		Ref<RenderTarget> RenderTarget = nullptr;

		CameraComponent() = default;
		CameraComponent(Ref<Olala::Camera> camera, Ref<Olala::RenderTarget> renderTarget = nullptr, bool isMain = true, bool isOn = true)
			: Camera(camera), RenderTarget(renderTarget), IsMain(isMain), IsOn(isOn) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct SpriteRendererComponent
	{
		glm::vec2 Size = glm::vec2(10, 10);
		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> Texture = nullptr;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(glm::vec2 size, glm::vec4 color)
			: Size(size), Color(color) {}
		SpriteRendererComponent(glm::vec2 size, glm::vec4 color, Ref<Texture2D> texture)
			: Size(size), Color(color), Texture(texture) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct Rigidbody2DComponent
	{
		glm::vec2 Velocity = glm::vec2(0.f);
		glm::vec2 CenterOfMass = glm::vec2(0.f);
		float AngularVelocity = 0.f;

		float Mass = 1.f;
		bool IsStatic = false;
		bool ApplyGravity = true;


		void* RuntimeBody = nullptr;


		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(float mass, bool applyGravity = true, bool isStatic = false)
			: Mass(mass), ApplyGravity(applyGravity), IsStatic(isStatic) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct Collider2DComponent
	{
		glm::vec2 Center = glm::vec2(0.f, 0.f);

		Collider2DComponent() = default;
		Collider2DComponent(glm::vec2 center = glm::vec2(0.f, 0.f))
			: Center(center) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct BoxCollider2DComponent : public Collider2DComponent
	{
		glm::vec2 Size = glm::vec2(10.f, 10.f);
		float AngularOffset = 0.f;

		BoxCollider2DComponent() : Collider2DComponent(glm::vec2(0.f, 0.f)) {}
		BoxCollider2DComponent(glm::vec2 size, glm::vec2 center = glm::vec2(0.f, 0.f))
			: Size(size), Collider2DComponent(center) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct CircleCollider2DComponent : public Collider2DComponent
	{
		float Radius = 1.f;

		CircleCollider2DComponent() : Collider2DComponent(glm::vec2(0.f, 0.f)) {}
		CircleCollider2DComponent(float radius, glm::vec2 center = glm::vec2(0.f, 0.f))
			: Radius(radius), Collider2DComponent(center) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct MeshRendererComponent
	{
		Ref<Mesh> Mesh;
		Ref<Material> Material;
		bool IsOn = true;

		MeshRendererComponent() {}
		MeshRendererComponent(Ref<::Olala::Mesh> mesh, Ref<::Olala::Material> material, bool isOn = true)
			: Mesh(mesh), Material(material), IsOn(isOn) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Direction = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 Color = glm::vec3(1.f);
		float Intensity = 1.f;
		bool CastShadow = true;

		DirectionalLightComponent() {}
		DirectionalLightComponent(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.f, bool castShadow = true)
			: Direction(direction), Color(color), Intensity(intensity), CastShadow(castShadow) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct PointLightComponent
	{
		glm::vec3 Color = glm::vec3(1.f);
		float Range = 1.f;
		float Intensity = 1.f;
		bool CastShadow = true;

		PointLightComponent() {}
		PointLightComponent(const glm::vec3& color, float range = 1.f, float intensity = 1.f, bool castShadow = true)
			: Color(color), Range(range), Intensity(intensity), CastShadow(castShadow) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct SpotLightComponent
	{
		glm::vec3 Direction = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 Color = glm::vec3(1.f);
		float Range = 1.f;
		float Angle = 30.f;
		float Intensity = 1.f;
		bool CastShadow = true;

		SpotLightComponent() {}
		SpotLightComponent(const glm::vec3& direction, const glm::vec3& color, float range = 1.f, float angle = 30.f,
			float intensity = 1.f, bool castShadow = true)
			: Direction(direction), Color(color), Range(range), Angle(angle), Intensity(intensity), CastShadow(castShadow) {}

		NOT_USER_DEFINED_SCRIPT;
	};

	struct SkyBoxComponent
	{
		Ref<CubeMap> SkyBox;
		glm::vec4 Color = glm::vec4(1.0f);

		SkyBoxComponent() {}
		SkyBoxComponent(Ref<CubeMap> skyBox, const glm::vec4& color = glm::vec4(1.0f))
			: SkyBox(skyBox), Color(color) {}

		NOT_USER_DEFINED_SCRIPT;
	};

}