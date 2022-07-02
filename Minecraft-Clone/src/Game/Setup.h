#pragma once

#include <Olala.h>
#include "ChunkManager.h"
#include "PlayerController.h"
using namespace Olala;



class Setup : public Script
{
	// Initialize the scene
	void OnCreate()
	{
		auto light = Instantiate("Sky");
		light.AddComponent<DirectionalLightComponent>(glm::vec3{ 0.f, -0.2f, -1.f }, glm::vec3{ 1.0f, 1.0f, 1.0f }, 5.f);
		light.AddComponent<SkyBoxComponent>(CubeMap::Create("../Olala/Asset/Texture/skybox"));


		auto player = Instantiate("Player");
		player.AddComponent<CameraComponent>(CreateRef<PerspectiveCamera>(), RenderTarget::Create(RenderMode::Forward));
		player.AddComponent<PlayerController>();
		
		
		auto world = Instantiate("World Generator");
		auto& manager = world.AddComponent<ChunkManager>();
		manager.SetFollowingEntity(player);
		manager.StartUp();


		auto donut = Instantiate("Donut");
		auto model = ColladaParser::Parse("../Olala/Asset/model/untitled.dae");
		if (model)
			donut.AddComponent<MeshRendererComponent>(model->GetMesh(), model->GetMaterial());


		Destroy();
	}

};