#include "Apple.h"

void Apple::Render(SDL_Renderer* renderer, Transform transform)
{
	// Set draw color
	SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);

	// Draw apple
	SDL_FRect rect = SDL_FRect(transform.Position.X, transform.Position.Y, transform.Scale, transform.Scale);
	SDL_RenderFillRectF(renderer, &rect);
}
