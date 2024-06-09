#include <iostream>
#include "Grab.h"
#include "../Core/Engine.h"

void Grab::HandleEvent(SDL_Event& e, Transform& transform) {
	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
		// Start grabbing when in range
		if (transform.Position.Distance(Engine::GetInstance()->GetWindow()->GetMouseLogicalPosition()) <= transform.Scale)
		{
			isGrabbing = true;
		}
	}
	else if (isGrabbing && e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		isGrabbing = false;
	}
	else if (isGrabbing && e.type == SDL_MOUSEMOTION) {
		// Get mouse position and set transform position
		transform.Position = Engine::GetInstance()->GetWindow()->GetMouseLogicalPosition();
	}
}
