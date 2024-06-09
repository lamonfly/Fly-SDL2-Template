#pragma once

#include <SDL.h>
#include "Renderable.h"

class Circle : Renderable
{
public:
	SDL_Color Color = { 0,0,0,255 };

	// Initializes variables
	Circle() = default;

	// Deallocates memorys
	~Circle() = default;

	// Render at given transform
	void Render(SDL_Renderer* renderer, Transform transform) override;
};

