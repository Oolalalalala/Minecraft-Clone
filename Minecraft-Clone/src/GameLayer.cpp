#include "GameLayer.h"

#include "Game/Setup.h"
using namespace Olala;

GameLayer::GameLayer()
	: Layer()
{
	m_Scene = CreateRef<Scene>();

	m_Scene->RegisterComponentSystem<PlayerControllerSystem>();
	m_Scene->RegisterComponentSystem<ChunkManagerSystem>();

	m_Scene->CreateEntity("Setup").AddComponent<Setup>();
}

GameLayer::~GameLayer()
{
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
	m_Scene->OnUpdate(dt);

	static int cnt = 0;
	static float tm = 0;
	cnt++;
	tm += dt;
	if (tm > 1.f)
	{
		OLA_TRACE("FPS : {0}", cnt);
		tm -= 1.f;
		cnt = 0;
	}
}

void GameLayer::OnEvent(Olala::Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
}

void GameLayer::OnImGuiRender()
{
	//return;

	auto view = m_Scene->GetAllEntitiesWith<TagComponent>();

	static bool isOpen = true;
	ImGui::Begin("Debug", &isOpen);

	std::vector<std::pair<const char*, entt::entity>> entities;
	for (auto e : view)
	{
		entities.push_back({ view.get<TagComponent>(e).Tag.c_str(), e });
	}
	static const char* currentItem = nullptr;
	static Entity selectedEntity;
	if (ImGui::BeginCombo("Entities", currentItem))
	{
		for (uint32_t i = 0; i < entities.size(); i++)
		{
			bool is_selected = (currentItem == entities[i].first);
			if (ImGui::Selectable(entities[i].first, is_selected))
			{
				currentItem = entities[i].first;
				selectedEntity = { entities[i].second, m_Scene.get() };
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	// Display info
	if (selectedEntity && ImGui::TreeNode("Transform"))
	{
		ImGui::DragFloat3("Position", (float*)&selectedEntity.GetComponent<TransformComponent>().Position);
		ImGui::DragFloat3("Rotation", (float*)&selectedEntity.GetComponent<TransformComponent>().Rotation);
		ImGui::DragFloat3("Scale", (float*)&selectedEntity.GetComponent<TransformComponent>().Scale);
		ImGui::TreePop();
	}
	ImGui::End();

	
}

bool GameLayer::OnWindowResize(WindowResizeEvent& e)
{
	uint32_t width = e.GetWidth(), height = e.GetHeight();
	for (auto [e, camera] : m_Scene->GetAllEntitiesWith<CameraComponent>().each())
	{
		if (camera.RenderTarget)
		{
			camera.RenderTarget->Resize(width, height);
		}
	}
	return false;
}
