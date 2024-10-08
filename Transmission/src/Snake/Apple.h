#pragma once

#include <Graphics/Renderable.h>

class Apple : Renderable
{
public:
	SDL_Color Color = { 255,0,0,255 };

	// Initializes variables
	Apple() = default;

	// Deallocates memorys
	~Apple() = default;

	// Draw apple
	void Render(SDL_Renderer* renderer, Transform transform) override;
};

