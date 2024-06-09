#pragma once

#include <SDL_ttf.h>
#include "Renderable.h"
#include "Texture.h"
#include "../Core/Engine.h"

class Text : Renderable
{
private:
	// Texture wrapper
	Texture* mTexture;

public:
	// Initializes variables
	Text() = default;
	Text(Texture* texture);

	//Deallocates memory
	~Text();

	void Render(SDL_Renderer* renderer, Transform transform) override;
};

