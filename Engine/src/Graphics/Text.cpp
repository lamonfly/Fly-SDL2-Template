#include "Text.h"

Text::Text(Texture* texture)
{
	mTexture = texture;
}

Text::~Text()
{
	//Deallocate
	mTexture->Free();
}

void Text::Render(SDL_Renderer* renderer, Transform transform)
{
	mTexture->Render(renderer, transform.Position.X, transform.Position.Y, nullptr, transform.getRotation(), 0, SDL_FLIP_NONE);
}
