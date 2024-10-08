#pragma once

#include <sdl.h>
#include "../Physics/Transform.h"
#include "Eventable.h"

class Grab : Eventable
{
private:
	bool isGrabbing = false;
public:
	void HandleEvent(SDL_Event& e, Transform& transform) override;
};

