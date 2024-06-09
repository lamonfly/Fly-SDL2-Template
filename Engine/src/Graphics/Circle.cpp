#include "Circle.h"
#include "../Physics/Transform.h"

void Circle::Render(SDL_Renderer* renderer, Transform transform) 
{
    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);

    for (int w = 0; w < transform.Scale * 2; w++)
    {
        for (int h = 0; h < transform.Scale * 2; h++)
        {
            float dx = transform.Scale - w; // horizontal offset
            float dy = transform.Scale - h; // vertical offset
            if ((dx * dx + dy * dy) <= (transform.Scale * transform.Scale))
            {
                SDL_RenderDrawPointF(renderer, transform.Position.X + dx, transform.Position.Y + dy);
            }
        }
    }
}