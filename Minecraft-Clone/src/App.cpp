#include <Olala.h>
#include <Core/EntryPoint.h>

#include "GameLayer.h"

class App : public Olala::Application
{
public:
	App()
		: Application(Olala::ApplicationSpecs{
			"Minecraft-Clone",                                           // Name
			1280, 720,                                                   // Size
			"",                                                          // Icon
			"",                                                          // Small Icon
			true                                                         // VSync
			})
	{
		PushLayer(new GameLayer());
	}

	~App()
	{

	}
};

Olala::Application* Olala::CreateApplication()
{
	return new App();
}