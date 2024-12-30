#pragma once
#include <Physics/Vector2.h>
#include <Physics/Transform.h>
#include <Event/Eventable.h>
#include <SDL_mixer.h>
#include <vector>

class BallMovement : Eventable
{
private:
	Vector2 Velocity;
	float Speed = 150;
public:
	void HandleEvent(SDL_Event& e) override;
	void UpdateMove(double deltaTime, Transform& transform);
	void Reflect(Vector2 normal);
};

