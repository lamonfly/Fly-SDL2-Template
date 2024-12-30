#pragma once

#include <SDL.h>
#include "../Physics/Vector2.h"

class Window
{
public:
	// Initializes internals
	Window();

	// Creates window
	bool Init();

	// Creates renderer from internal window
	SDL_Renderer* CreateRenderer();

	// Handles window events
	void HandleEvent(SDL_Event& e);

	// Deallocates internals
	void Free();

	// Window dimensions
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }

	// Window dimensions
	int GetResolutionWidth() { return mResolutionWidth; }
	int GetResolutionHeight() { return mResolutionHeight; }

	// Window renderer
	inline SDL_Renderer* GetRenderer() { return mRenderer; }

	// Window focus
	bool isMinimized() { return mMinimized; }

	// Window Background Color
	SDL_Color Color = { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF };

	// Mouse Position
	inline Vector2 GetMouseLogicalPosition() { return mMouseLogicalPosition; }

private:
	// Window data
	SDL_Window* mWindow;

	// Window renderer
	SDL_Renderer* mRenderer;

	// Window dimensions
	int mWidth;
	int mHeight;

	// Resolution dimensions
	int mResolutionWidth;
	int mResolutionHeight;

	// Window focus
	bool mFullScreen;
	bool mMinimized;

	// Mouse position
	int mMouseX, mMouseY;
	Vector2 mMouseLogicalPosition;
};