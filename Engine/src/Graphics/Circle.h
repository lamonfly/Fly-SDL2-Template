#pragma once

#include <SDL.h>
#include "Renderable.h"

class Circle : Renderable
{
public:
	float Radius = 1;
	SDL_Color Color = { 0,0,0,255 };

	// Initializes variables
	Circle() = default;
	Circle(float radius);

	// Deallocates memorys
	~Circle() = default;

	// Render at given transform
	void Render(SDL_Renderer* renderer, Transform transform) override;
};

