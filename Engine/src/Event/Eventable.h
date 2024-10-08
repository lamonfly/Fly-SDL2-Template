#pragma once

#include <SDL.h>
#include "../Physics/Transform.h"

class Eventable {
public:
	virtual void HandleEvent(SDL_Event& e, Transform& transform) = 0;
};