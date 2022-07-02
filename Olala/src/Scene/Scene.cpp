#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Component.h"
#include "ComponentSystem.h"
#include "Core/IO.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/UniformBuffer.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Renderer/Effect/Effect.h"


namespace Olala {

	Scene::Scene()
	{
		m_AssetManager = CreateRef<AssetManager>();
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(float dt)
	{
		// Component Systems
		for (auto cs : m_ComponentSystems)
		{
			cs->OnUpdate(dt);
		}

		// Get All Cameras
		std::vector<Entity> cameras;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto e : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(e);
				camera.Camera->SetPosition(transform.Position);
				camera.Camera->SetDirection(glm::toMat4(glm::quat(glm::radians(transform.Rotation))) * glm::vec4(0.f, 0.f, 1.f, 0.0f));
				if (camera.IsOn && camera.RenderTarget)
					cameras.emplace_back(e, this);
			}
		}

		// Render the scene to all cameras
		{
			for (Entity& camera : cameras)
			{
				auto& cameraComponent = camera.GetComponent<CameraComponent>();

				RenderCommand::SetRenderTarget(cameraComponent.RenderTarget);
				RenderCommand::SetClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
				RenderCommand::Clear();

				struct CameraUniformData
				{
					glm::mat4 ViewProjectionmatrix;
					glm::vec4 Position;
				};
				static auto cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraUniformData), 0);
				static CameraUniformData cameraUniformData;

				cameraUniformData.ViewProjectionmatrix = cameraComponent.Camera->GetViewProjectionMatrix();
				cameraUniformData.Position = glm::vec4(camera.GetComponent<TransformComponent>().Position, 0.f);
				cameraUniformBuffer->SetData(&cameraUniformData, sizeof(CameraUniformData));

				// Renderer3D
				{
					Renderer3D::BeginScene(cameraComponent.Camera);

					// Objects
					{
						auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
						for (auto e : view)
						{
							auto& transform = view.get<TransformComponent>(e);
							auto& mesh = view.get<MeshRendererComponent>(e);

							if (mesh.IsOn)
								Renderer3D::Submit(mesh.Mesh, mesh.Material, transform.GetTransform());
						}
					}

					// Directional Light
					{
						auto view = m_Registry.view<TransformComponent, DirectionalLightComponent>();
						for (auto& e : view)
						{
							auto& light = view.get<DirectionalLightComponent>(e);
							Renderer3D::AddDirectionalLight(light.Direction, light.Color, light.Intensity, light.CastShadow);
						}
					}

					// Point Light
					{
						auto view = m_Registry.view<TransformComponent, PointLightComponent>();
						for (auto e : view)
						{
							auto& [transform, light] = view.get<TransformComponent, PointLightComponent>(e);
							Renderer3D::AddPointLight(transform.Position, light.Color, light.Range, light.Intensity, light.CastShadow);
						}
					}

					// Spot Light
					{
						auto view = m_Registry.view<TransformComponent, SpotLightComponent>();
						for (auto e : view)
						{
							auto& [transform, light] = view.get<TransformComponent, SpotLightComponent>(e);
							Renderer3D::AddSpotLight(transform.Position, light.Direction, light.Color,
								                     light.Angle, light.Range, light.Intensity, light.CastShadow);
						}
					}

					// Sky Box
					{
						auto view = m_Registry.view<SkyBoxComponent>();
						for (auto e : view)
						{
							auto& skyBox = view.get<SkyBoxComponent>(e);
							Renderer3D::SetSkyBox(skyBox.SkyBox, skyBox.Color);
						}
					}

					Renderer3D::EndScene();
				}

				// Renderer2D
				{
					Renderer2D::BeginScene(cameraComponent.Camera);
					auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
					for (auto e : view)
					{
						auto& transform = view.get<TransformComponent>(e);
						auto& sprite = view.get<SpriteRendererComponent>(e);

						Renderer2D::DrawQuad((glm::vec2)transform.Position, sprite.Size * (glm::vec2)transform.Scale, transform.Rotation.z, sprite.Texture, sprite.Color);
					}
					Renderer2D::EndScene();
				}
			}

		}

		static auto displayShader = Shader::Create("../Olala/Asset/Internal/Shader/Display.glsl");

		// Post processing
		for (auto& entity : cameras)
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (camera.IsMain)
			{
				Effect::Bloom(camera.RenderTarget);
				Effect::ToneMapping(camera.RenderTarget);

				RenderCommand::SetRenderTargetDefaultFramebuffer();
				RenderCommand::Clear();
				displayShader->Bind();
				displayShader->SetUniformInt("u_Texture", 0);
				camera.RenderTarget->GetFramebuffer()->BindColorAttachment(0, 0);
				RenderCommand::DrawFillingQuad();
			}
		}


		// Destory entities and remove components
		for (Entity entity : m_EntityDestroyList)
			DestroyEntity(entity);

		for (auto& removeFunc : m_ComponentRemoveList)
			removeFunc();

		m_EntityDestroyList.clear();
		m_ComponentRemoveList.clear();
	}

	void Scene::OnUpdateRuntime(float dt)
	{
		// Physics
		{
			m_PhysicsWorld->Step(dt, 6, 2);

			auto view = m_Registry.view<TransformComponent, Rigidbody2DComponent>();
			for (auto e : view)
			{
				auto [transform, rb2d] = view.get<TransformComponent, Rigidbody2DComponent>(e);
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				
				transform.Position.x = body->GetPosition().x;
				transform.Position.y = body->GetPosition().y;
				transform.Rotation.z = glm::degrees(body->GetAngle());
				rb2d.Velocity.x = body->GetLinearVelocity().x;
				rb2d.Velocity.y = body->GetLinearVelocity().y;
				rb2d.AngularVelocity = glm::degrees(body->GetAngularVelocity());
			}
			
		}

		// Get All Cameras
		std::vector<Entity> cameras;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				camera.Camera->SetPosition(transform.Position);
				camera.Camera->SetDirection(glm::inverse(glm::toMat4(glm::quat(glm::radians(transform.Rotation)))) * glm::vec4(0.f, 0.f, 1.f, 0.0f));

				if (camera.IsOn && camera.RenderTarget != nullptr)
					cameras.emplace_back(entity, this);
			}
		}

		// Render the scene to all camera
		{
			auto group = m_Registry.group<TransformComponent, SpriteRendererComponent>();

			for (Entity& camera : cameras)
			{
				auto& cameraComponent = camera.GetComponent<CameraComponent>();

				RenderCommand::SetRenderTarget(cameraComponent.RenderTarget);
				RenderCommand::SetClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
				RenderCommand::Clear();

				Renderer2D::BeginScene(cameraComponent.Camera);

				for (auto entity : group)
				{
					auto& transform = group.get<TransformComponent>(entity);
					auto& sprite = group.get<SpriteRendererComponent>(entity);

					Renderer2D::DrawQuad((glm::vec2)transform.Position, sprite.Size * (glm::vec2)transform.Scale, transform.Rotation.z, sprite.Texture, sprite.Color);
				}

				Renderer2D::EndScene();
			}

			RenderCommand::SetRenderTargetDefaultFramebuffer();
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create() , this);
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (m_PhysicsWorld && entity.HasComponent<Rigidbody2DComponent>())
			m_PhysicsWorld->DestroyBody((b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody);
		m_Registry.destroy(entity.m_EntityID);
	}

	void Scene::InitializePhysics()
	{
		m_PhysicsWorld = CreateRef<b2World>(b2Vec2(0.f, -9.81f));

		auto view = m_Registry.view<TransformComponent, Rigidbody2DComponent>();
		for (auto e : view)
		{
			auto [transform, rb2d] = view.get<TransformComponent, Rigidbody2DComponent>(e);
			Entity entity(e, this);

			b2BodyDef def;
			def.position.Set(transform.Position.x, transform.Position.y);
			def.angle = glm::radians(transform.Rotation.z);
			def.linearVelocity.Set(rb2d.Velocity.x, rb2d.Velocity.y);
			def.angularVelocity = glm::radians(rb2d.AngularVelocity);
			def.gravityScale = rb2d.ApplyGravity ? 1.f : 0.f;
			def.type = rb2d.IsStatic ? b2BodyType::b2_staticBody : b2BodyType::b2_dynamicBody;

			b2MassData massData;
			massData.mass = rb2d.Mass;
			massData.center.Set(rb2d.CenterOfMass.x, rb2d.CenterOfMass.y);
			massData.I = 1.f; // temp

			// TODO : add properties to rigidbodycomponent
			b2FixtureDef fixtureDef;
			fixtureDef.density;
			fixtureDef.friction;
			fixtureDef.restitution = 0.6f;
			fixtureDef.restitutionThreshold = 10.f;

			b2Body* body = m_PhysicsWorld->CreateBody(&def);
			body->SetMassData(&massData);

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				b2CircleShape circle;
				circle.m_p.Set(cc2d.Center.x, cc2d.Center.y);
				circle.m_radius = cc2d.Radius;
				

				fixtureDef.shape = &circle;
				body->CreateFixture(&fixtureDef);
			}
			else if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape box;
				box.SetAsBox(bc2d.Size.x * transform.Scale.x * 0.5f, bc2d.Size.y * transform.Scale.y * 0.5f, b2Vec2(bc2d.Center.x, bc2d.Center.y), glm::radians(bc2d.AngularOffset));

				fixtureDef.shape = &box;
				body->CreateFixture(&fixtureDef);
			}


			rb2d.RuntimeBody = body;
		}

	}

	void Scene::Reset()
	{
		m_Name = "Untitled";
		m_Registry.clear();
		m_PhysicsWorld = nullptr;
		m_AssetManager = CreateRef<AssetManager>();
	}

	Ref<Scene> Scene::Copy(Ref<Scene> source)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_Name = source->m_Name;

		auto tagView = source->m_Registry.view<TagComponent>();
		for (auto e : tagView)
		{
			Entity newEntity = newScene->CreateEntity(tagView.get<TagComponent>(e).Tag);
			Entity oldEntity(e, source.get());

			if (oldEntity.HasComponent<TransformComponent>())        newEntity.GetComponent<TransformComponent>()        = oldEntity.GetComponent<TransformComponent>();
			if (oldEntity.HasComponent<SpriteRendererComponent>())   newEntity.AddComponent<SpriteRendererComponent>()   = oldEntity.GetComponent<SpriteRendererComponent>();
			if (oldEntity.HasComponent<CircleCollider2DComponent>()) newEntity.AddComponent<CircleCollider2DComponent>() = oldEntity.GetComponent<CircleCollider2DComponent>();
			if (oldEntity.HasComponent<BoxCollider2DComponent>())    newEntity.AddComponent<BoxCollider2DComponent>()    = oldEntity.GetComponent<BoxCollider2DComponent>();


			if (oldEntity.HasComponent<CameraComponent>())
			{
				auto& camera = newEntity.AddComponent<CameraComponent>() = oldEntity.GetComponent<CameraComponent>();
				camera.RenderTarget = RenderTarget::Create(camera.RenderTarget->GetRenderMode());
			}

			if (oldEntity.HasComponent<Rigidbody2DComponent>())
			{
				auto& rb2d = newEntity.AddComponent<Rigidbody2DComponent>() = oldEntity.GetComponent<Rigidbody2DComponent>();
				//rb2d.PhysicsHandle = 0;
			}
		}

		return newScene;
	}

}
