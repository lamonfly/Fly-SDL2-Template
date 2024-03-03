#pragma once

#include "Texture.h"
#include "../Physics/Transform.h"

#include <string>

class Sprite
{
private:
	// Texture wrapper
	Texture* mTexture;

	// Flip texture
	SDL_RendererFlip mFlip;
public:
	//Initializes variables
	Sprite() = default;
	Sprite(Texture* texture);

	//Deallocates memory
	~Sprite();

	//Set sprite flip
	void SetFlip(SDL_RendererFlip flip);

	//Renders at given transform
	void Render(Transform transform);
};