#pragma once

#include <SDL.h>

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

	// Window focus
	bool hasMouseFocus() { return mMouseFocus; }
	bool hasKeyboardFocus() { return mKeyboardFocus; }
	bool isMinimized() { return mMinimized; }

private:
	// Window data
	SDL_Window* mWindow;

	// Window renderer
	SDL_Renderer* mRenderer;

	// Window dimensions
	int mWidth;
	int mHeight;

	// Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};