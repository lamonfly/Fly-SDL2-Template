#pragma once

#include "Texture.h"
#include "../Physics/Transform.h"

#include <string>

class Sprite
{
private:
	// Texture wrapper
	Texture* mTexture;

public:
	//Initializes variables
	Sprite() = default;
	Sprite(Texture* texture);

	//Deallocates memory
	~Sprite();

	//Renders at given transform
	void Render(SDL_Renderer* renderer, Transform transform);
};