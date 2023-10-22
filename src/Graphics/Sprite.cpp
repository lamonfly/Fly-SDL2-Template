#include "Sprite.h"

#include <SDL_image.h>

Sprite::Sprite(Texture* texture)
{
	mTexture = texture;
}

Sprite::~Sprite()
{
	//Deallocate
	mTexture->Free();
}

void Sprite::Render(SDL_Renderer* renderer, Transform transform) {
	mTexture->Render(transform.Position.X, transform.Position.Y, renderer);
}