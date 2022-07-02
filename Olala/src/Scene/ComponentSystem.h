#pragma once

#include "Scene.h"

namespace Olala {

	class ComponentSystem
	{
	public:
		virtual void OnUpdate(float dt) {};
		virtual void OnLateUpdate(float dt) {};
		virtual void OnFixedUpdate() {};

	protected:
		template<typename ... Components>
		auto GetEntities()
		{
			return m_Scene->GetAllEntitiesWith<Components...>().each();
		}


	private:
		Scene* m_Scene;
		friend class Scene;
	};

}