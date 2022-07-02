#pragma once

#include "entt/entt.hpp"
#include "AssetManager.h"

#include "box2d/include/box2d/box2d.h"



class SceneHierarchyPanel;

namespace Olala {

	class Entity;
	class ComponentSystem;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(float dt);
		void OnUpdateRuntime(float dt);

		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void InitializePhysics();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		template<typename T, typename ... TT>
		void RegisterComponentSystem()
		{
			Ref<ComponentSystem> cs = std::static_pointer_cast<ComponentSystem>(CreateRef<T>());
			cs->m_Scene = this;
			m_ComponentSystems.emplace_back(cs);
		}

		template<typename T>
		void RemoveComponentFunction(entt::entity e)
		{
			m_Registry.remove<T>(e);
		}

		void Reset();

		Ref<AssetManager> GetAssetManager() { return m_AssetManager; }
		std::string& GetName() { return m_Name; }

		static Ref<Scene> Copy(Ref<Scene> source);

	private:
		entt::registry m_Registry;
		std::vector<Ref<ComponentSystem>> m_ComponentSystems;

		std::vector<Entity> m_EntityDestroyList;
		std::vector<std::function<void()>> m_ComponentRemoveList;

		Ref<AssetManager> m_AssetManager;
		std::string m_Name = "Untitled";

		Ref<b2World> m_PhysicsWorld;

	private:
		friend class Entity;
		friend class Script;
		friend class SceneSerializer;
		friend class ::SceneHierarchyPanel;
	};

}

