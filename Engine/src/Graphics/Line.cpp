#include "Line.h"

Line::Line(Vector2 start, Vector2 end)
{
	Start = start;
	End = end;
}

void Line::Render(SDL_Renderer* renderer, Transform transform) {
	SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
	SDL_RenderDrawLineF(renderer, Start.X + transform.Position.X, Start.Y + transform.Position.Y, End.X + transform.Position.X, End.Y + transform.Position.Y);
}