#include "playerPlatform.h"

void PlayerPlatform::HandleEvent(SDL_Event& e, Transform& transform)
{
	int currentDirection = 0;

	if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.sym == SDLK_LEFT) {
			currentDirection -= 1;
		}
		if (e.key.keysym.sym == SDLK_RIGHT) {
			currentDirection += 1;
		}
	}

	direction = currentDirection;
}
