#pragma once

#include "entt/entt.hpp"
#include "Scene.h"
#include "Component.h"


namespace Olala {

	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity entityID, Scene* scene)
			: m_EntityID(entityID), m_Scene(scene) {}

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			if constexpr (T::IsScript())
			{
				T& component = m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
				Script* script = static_cast<Script*>(&component);
				script->m_Entity = *this;
				script->OnCreate();
				return component;
			}
			else
				return m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			// OnDestroy is called at destructor
			m_Scene->m_Registry.remove<T>(m_EntityID);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Scene->m_Registry.get<T>(m_EntityID);
		}

		template<typename ... Args>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<Args...>(m_EntityID);
		}

		template<typename ... Args>
		bool HasAnyComponent()
		{
			return m_Scene->m_Registry.any<Args...>(m_EntityID);
		}

		// TODO : turn these two function to GetScene()
		Ref<b2World> GetPhysicsWorld()
		{
			return m_Scene->m_PhysicsWorld;
		}
		b2Body& GetPhysicsBody()
		{
			return *(b2Body*)m_Scene->m_Registry.get<Rigidbody2DComponent>(m_EntityID).RuntimeBody;
		}

		bool IsValid() { return m_Scene && m_Scene->m_Registry.valid(m_EntityID); }

		operator bool() { return m_Scene && m_Scene->m_Registry.valid(m_EntityID); }

		bool operator==(const Entity& other)
		{
			return m_EntityID == other.m_EntityID && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other)
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityID = entt::null;
		Scene* m_Scene = nullptr;

		friend class Scene;
		friend class Script;
	};


	class Script
	{
	public:
		~Script() { OnDestroy(); };
		bool IsEnabled() { return m_Enabled; }
		void SetEnabled(bool enable) { m_Enabled = enable; }

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}

	private:
		Entity m_Entity;
		bool m_Enabled = true;
		bool m_ShouldBeRemoved = false;
		friend class Scene;
		friend class Entity;

	public:

		constexpr static bool IsScript() { return true; }

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Entity.m_Scene->m_ComponentRemoveList.emplace_back(
				std::bind(&Scene::RemoveComponentFunction<T>, m_Entity.m_Scene, m_Entity.m_EntityID));
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}

		Entity Instantiate(const std::string& name)
		{
			return m_Entity.m_Scene->CreateEntity(name);
		}

		void Destroy()
		{
			m_Entity.m_Scene->m_EntityDestroyList.emplace_back(m_Entity);
		}

		void Destroy(Entity entity)
		{
			entity.m_Scene->m_EntityDestroyList.emplace_back(entity);
		}

	};


}