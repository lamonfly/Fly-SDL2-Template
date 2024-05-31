#pragma once

#include <sdl.h>
#include "../Physics/Transform.h"

class Grab
{
private:
	bool isGrabbing = false;
public:
	void HandleEvent(SDL_Event& e, Transform& transform);
};

