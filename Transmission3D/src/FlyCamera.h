#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <algorithm>
#include "Graphics3D/Camera3D.h"

// WASD + QE, Shift = fast, mouse look while captured
struct FlyCamera
{
	glm::vec3 Position = glm::vec3(0.0f, 5.0f, 15.0f);
	float Yaw = -90.0f;
	float Pitch = -15.0f;
	float Speed = 6.0f;          // m/s
	float Sensitivity = 0.1f;    // deg/px
	bool Captured = false;

	void OnMouseMotion(int xrel, int yrel)
	{
		if (!Captured) return;
		Yaw += xrel * Sensitivity;
		Pitch = std::clamp(Pitch - yrel * Sensitivity, -89.0f, 89.0f);
	}

	void Update(float dt, const Uint8* keys)
	{
		Camera3D cam;
		Apply(cam);
		glm::vec3 forward = cam.Forward();
		glm::vec3 right = cam.Right();
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		float speed = Speed * ((keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) ? 3.0f : 1.0f);
		glm::vec3 move(0.0f);
		if (keys[SDL_SCANCODE_W]) move += forward;
		if (keys[SDL_SCANCODE_S]) move -= forward;
		if (keys[SDL_SCANCODE_D]) move += right;
		if (keys[SDL_SCANCODE_A]) move -= right;
		if (keys[SDL_SCANCODE_E]) move += up;
		if (keys[SDL_SCANCODE_Q]) move -= up;

		float len = glm::length(move);
		if (len > 0.0f) Position += move / len * speed * dt;
	}

	glm::vec3 Forward() const
	{
		Camera3D cam;
		Apply(cam);
		return cam.Forward();
	}

	void Apply(Camera3D& cam) const
	{
		cam.Position = Position;
		cam.Yaw = Yaw;
		cam.Pitch = Pitch;
	}
};
