#include "Window.h"

#include <iostream>
#include <sstream>

Window::Window() 
{
	// Initialize non-existant window
	mWindow = NULL;
	mRenderer = nullptr;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
	mResolutionWidth = 320;
	mResolutionHeight = 240;
}

bool Window::Init() 
{
	// Get window size
	SDL_Rect Re;
	if (SDL_GetDisplayUsableBounds(0, &Re) < 0)
	{
		printf("SDL could not get display usable bounds! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Create window
	mWindow = SDL_CreateWindow(_TARGETNAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Re.w, Re.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = Re.w;
		mHeight = Re.h;
	}

	return mWindow != NULL;
}

SDL_Renderer* Window::CreateRenderer() 
{
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Set resolution
	SDL_RenderSetLogicalSize(mRenderer, mResolutionWidth, mResolutionHeight);
	SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);

	return mRenderer;
}

void Window::HandleEvent(SDL_Event& e) 
{
	// Window event occured
	if (e.type == SDL_WINDOWEVENT) 
	{
		// Caption update flag
		bool updateCaption = false;
		
		switch (e.window.event)
		{
			// Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(mRenderer);
			break;

			// Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(mRenderer);
			break;

			// Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			// Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			// Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			// Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			// Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			// Window maximized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			// Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}

		// Update window caption with new data
		if (updateCaption)
		{
			std::stringstream caption;
			caption << _TARGETNAME <<" - MouseFocus:" << ((mMouseFocus) ? "On" : "Off") << " KeyboardFocus:" << ((mKeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}
	}
	// Enter exit full screen on return key
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
	{
		if (mFullScreen)
		{
			SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
			mFullScreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
			mFullScreen = true;
			mMinimized = false;
		}
	}
}

void Window::Free() 
{
	// Destory window and renderer
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);

	// Deallocates internals
	mWindow = NULL;
	mRenderer = NULL;
}