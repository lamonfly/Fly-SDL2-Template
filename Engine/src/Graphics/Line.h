#pragma once

#include "../Physics/Vector2.h"
#include <SDL.h>

class Line
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
	void Render(SDL_Renderer* renderer, Vector2 position);
};

