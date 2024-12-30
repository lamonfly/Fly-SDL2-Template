#include "Sprite.h"

#include <SDL_image.h>

Sprite::Sprite(Texture* texture)
{
	mTexture = texture;
	mFlip = SDL_FLIP_NONE;
	mClip = nullptr;
}

Sprite::~Sprite()
{
	//Deallocate
	mTexture->Free();
}

void Sprite::SetFlip(SDL_RendererFlip flip) {
	mFlip = flip;
}

void Sprite::SetTexture(Texture* texture)
{
	mTexture = texture;
}

void Sprite::SetClip(SDL_Rect* clip)
{
	mClip = clip;
}

void Sprite::Render(SDL_Renderer* renderer, Transform transform) {
	mTexture->Render(renderer, transform.Position.X, transform.Position.Y, mClip, transform.getRotation(), 0, mFlip);
}