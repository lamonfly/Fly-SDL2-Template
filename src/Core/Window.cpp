#include "Window.h"

#include <iostream>
#include <sstream>
#include <libloaderapi.h>

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
}

bool Window::Init() 
{
	// Get window size
	SDL_DisplayMode DM;
	if (SDL_GetCurrentDisplayMode(0, &DM) < 0)
	{
		printf("SDL could not get current display mode! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// TODO: Set window name to correct name
	// Create window
	mWindow = SDL_CreateWindow(__FILE__, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DM.w, DM.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = DM.w;
		mHeight = DM.h;
	}

	if (!mFullScreen) 
	{
		SDL_MaximizeWindow(mWindow);
	}

	return mWindow != NULL;
}

SDL_Renderer* Window::CreateRenderer() 
{
	// TODO: Keep renderer stored in window and not in engine
	return mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
			caption << __FILE__ <<" - MouseFocus:" << ((mMouseFocus) ? "On" : "Off") << " KeyboardFocus:" << ((mKeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}\


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