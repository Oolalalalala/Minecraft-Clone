#pragma once
#include <Olala.h>
using namespace Olala;

class PlayerController : public Script
{
public:
	float speed = 10;
	float prevX = 0.f, prevY = 0.f;
	bool mouseMoving = false;
};

class PlayerControllerSystem : public ComponentSystem
{
	void OnUpdate(float dt) override
	{
		for (auto [e, transform, camera, controller] : GetEntities<TransformComponent, CameraComponent, PlayerController>())
		{
			glm::vec3 upDirection = glm::normalize(camera.Camera->GetUpDirection());
			glm::vec3 direction = camera.Camera->GetDirection();
			direction = glm::normalize(glm::vec3(direction.x, 0, direction.z));

			if (IO::IsKeyPressed(GLFW_KEY_W))
				transform.Position += direction * controller.speed * dt;
			if (IO::IsKeyPressed(GLFW_KEY_S))
				transform.Position -= direction * controller.speed * dt;
			if (IO::IsKeyPressed(GLFW_KEY_D))
				transform.Position += glm::cross(direction, upDirection) * controller.speed * dt;
			if (IO::IsKeyPressed(GLFW_KEY_A))
				transform.Position -= glm::cross(direction, upDirection) * controller.speed * dt;
			if (IO::IsKeyPressed(GLFW_KEY_SPACE))
				transform.Position.y += controller.speed * dt;
			if (IO::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
				transform.Position.y -= controller.speed * dt;

			// Camera movement
			if (IO::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
			{
				auto [x, y] = IO::GetMousePosition();
				if (controller.mouseMoving)
				{
					transform.Rotation.y -= (x - controller.prevX) * 0.1;
					transform.Rotation.x += (y - controller.prevY) * 0.1;
				}
				controller.prevX = x, controller.prevY = y;
				controller.mouseMoving = true;
			}
			else
				controller.mouseMoving = false;
		}
	}
};