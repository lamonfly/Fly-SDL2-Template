#include <iostream>
#include "Grab.h"
#include "../Core/Engine.h"

void Grab::HandleEvent(SDL_Event& e, Transform& transform) {
	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
		isGrabbing = true;
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		isGrabbing = false;
	}
	else if (isGrabbing && e.type == SDL_MOUSEMOTION) {
		// Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		float lx, ly;
		SDL_RenderWindowToLogical(Engine::GetInstance()->GetWindow()->GetRenderer(), x, y, &lx, &ly);

		// Set tranform position
		transform.Position.X = lx;
		transform.Position.Y = ly;
	}
}
