#include "pch.h"
#include "Application.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Effect/Effect.h"


namespace Olala {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecs& specs)
		: m_ApplicationName(specs.Name)
	{
		s_Instance = this;
		Init(specs);
		OLA_CORE_TRACE("Main loop begins");
	}

	void Application::Init(const ApplicationSpecs& specs)
	{
		Log::Init();
		OLA_CORE_TRACE("Application begins");
		OLA_CORE_TRACE("Application Initializing");

		WindowSpecs windowSpecs(specs.Name, specs.Width, specs.Height, specs.VSync);
		windowSpecs.IconImagePath = specs.IconImagePath;
		windowSpecs.SmallIconImagePath = specs.SmallIconImagePath;

		m_Window = Window::Create(windowSpecs);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_Timer = CreateScope<Timer>();

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
		Effect::Init();

		m_LayerStack = CreateScope<LayerStack>();
		m_ImGuiLayer = new ImGuiLayer();
		m_LayerStack->PushOverlay(m_ImGuiLayer);

		m_Running = true;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float dt = m_Timer->GetDeltaTime();
			for (Layer* layer : *m_LayerStack)
			{
				layer->OnUpdate(dt);
			}
			

			m_ImGuiLayer->Begin();
			for (Layer* layer : *m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
		Renderer2D::ShutDown();
		Renderer3D::ShutDown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); it++)
		{
			(*it)->OnEvent(e);
			if (e.Handled()) break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		return false;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack->PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack->PushOverlay(overlay);
	}

	Application& Application::Get()
	{
		return *s_Instance;
	}

}