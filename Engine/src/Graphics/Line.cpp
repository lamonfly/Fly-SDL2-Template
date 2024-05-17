#include "Line.h"

Line::Line(Vector2 start, Vector2 end)
{
	Start = start;
	End = end;
}

void Line::Render(SDL_Renderer* renderer, Vector2 position) {
	SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
	SDL_RenderDrawLineF(renderer, Start.X + position.X, Start.Y + position.Y, End.X + position.X, End.Y + position.Y);
}