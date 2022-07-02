#pragma once

#include <Olala.h>
using namespace Olala;



class GameLayer : public Olala::Layer
{
public:
	GameLayer();
	~GameLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnEvent(Event& e) override;
	void OnImGuiRender() override;

private:
	bool OnWindowResize(WindowResizeEvent& e);

private:
	Ref<Scene> m_Scene;
	Ref<VertexArray> m_Vao;
	Ref<Shader> m_Shader;
};