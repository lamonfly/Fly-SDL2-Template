#include "Circle.h"
#include "../Physics/Transform.h"

Circle::Circle(float radius) 
{
	Radius = radius;
}

void Circle::Render(SDL_Renderer* renderer, Transform transform) 
{
    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
    for (int w = 0; w < Radius * 2; w++)
    {
        for (int h = 0; h < Radius * 2; h++)
        {
            int dx = Radius - w; // horizontal offset
            int dy = Radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (Radius * Radius))
            {
                SDL_RenderDrawPointF(renderer, transform.Position.X + dx, transform.Position.Y + dy);
            }
        }
    }
}