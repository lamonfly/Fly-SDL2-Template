#pragma once

#include <SDL.h>

class Updateable {
public:
	virtual void Update(double deltaTime) = 0;
};