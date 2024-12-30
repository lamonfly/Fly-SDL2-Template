#include "BallMovement.h"
#include <SDL.h>

void BallMovement::HandleEvent(SDL_Event& e)
{
	if (Velocity.Magnitude() <= 0 && e.type == SDL_KEYDOWN)
	{
		Velocity = Vector2(rand() % 201 + (-100), rand()%101).Normalized();
	}
}

void BallMovement::UpdateMove(double deltaTime, Transform& transform)
{
	transform.Position += Velocity * Speed * deltaTime;
}

void BallMovement::Reflect(Vector2 normal)
{
	Velocity = Velocity - normal * 2 * (Velocity.Dot(normal));
	Speed *= 1.01f;
}
