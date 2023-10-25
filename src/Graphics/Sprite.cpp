#include "Sprite.h"

#include <SDL_image.h>

Sprite::Sprite(Texture* texture)
{
	mTexture = texture;
	mFlip = SDL_FLIP_NONE;
}

Sprite::~Sprite()
{
	//Deallocate
	mTexture->Free();
}

void Sprite::SetFlip(SDL_RendererFlip flip) {
	mFlip = flip;
}

void Sprite::Render(SDL_Renderer* renderer, Transform transform) {
	mTexture->Render(renderer, transform.Position.X, transform.Position.Y, nullptr, transform.Rotation, 0, mFlip);
}