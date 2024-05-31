#pragma once

#include <SDL.h>
#include "../Physics/Transform.h"

class Renderable {
public:
	virtual void Render(SDL_Renderer* renderer, Transform transform) = 0;
};
