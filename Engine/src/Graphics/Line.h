#pragma once

#include <SDL.h>
#include "Renderable.h"

class Line : Renderable
{
public:
	Vector2 Start = { 0, 0 };
	Vector2 End = { 0, 0 };
	SDL_Color Color = { 0,0,0,255 };

	// Initializes variables
	Line() = default;
	Line(Vector2 start, Vector2 end);

	// Deallocates memorys
	~Line() = default;

	// Render at given transform
	void Render(SDL_Renderer* renderer, Transform transform) override;
};

