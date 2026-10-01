#pragma once

#include <SDL.h>
#include <Event/Eventable.h>
#include <Physics/Updateable.h>

class PlayerPlatform
{
private:
	int direction = 0;
	int speed = 500;
	int maxPosition;
public:
	// Initializes variables
	PlayerPlatform(int position) : maxPosition(position) {};

	// Deallocates memorys
	~PlayerPlatform() = default;

	int GetDirection() { return direction; }

	int GetSpeed() { return speed; }

	int GetMaxPosition() { return maxPosition; }

	void HandleEvent(SDL_Event& e, Transform& transform);
};
