#include <glad/glad.h>
#include "Window.h"

#include <iostream>
#include <sstream>

Window::Window()
{
	// Initialize non-existant window
	mWindow = NULL;
	mRenderer = nullptr;
	mGLContext = nullptr;
	mBackend = RenderBackend::SDL2D;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
	mResolutionWidth = 720;
	mResolutionHeight = 480;
}

bool Window::Init(const EngineConfig& config)
{
	mConfig = config;
	mBackend = config.Backend;

	// Get window size
	SDL_Rect Re;
	if (SDL_GetDisplayUsableBounds(0, &Re) < 0)
	{
		printf("SDL could not get display usable bounds! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

	// GL attributes before window creation
	if (mBackend == RenderBackend::OpenGL3)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config.DepthBits);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		if (config.MSAASamples > 0)
		{
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.MSAASamples);
		}
		flags |= SDL_WINDOW_OPENGL;
	}

	// Create window
	mWindow = SDL_CreateWindow(_TARGETNAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Re.w, Re.h, flags);
	if (mWindow != NULL)
	{
		mWidth = Re.w;
		mHeight = Re.h;
	}

	return mWindow != NULL;
}

SDL_Renderer* Window::CreateRenderer()
{
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Set resolution
	SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);

	return mRenderer;
}

SDL_GLContext Window::CreateGLContext()
{
	mGLContext = SDL_GL_CreateContext(mWindow);
	if (mGLContext == nullptr)
	{
		printf("GL context could not be created! SDL Error: %s\n", SDL_GetError());
		return nullptr;
	}

	if (SDL_GL_MakeCurrent(mWindow, mGLContext) != 0)
	{
		printf("GL context could not be made current! SDL Error: %s\n", SDL_GetError());
		SDL_GL_DeleteContext(mGLContext);
		mGLContext = nullptr;
		return nullptr;
	}

	// Swap interval after MakeCurrent. 1, adaptive, then off
	if (mConfig.VSync)
	{
		if (SDL_GL_SetSwapInterval(1) != 0 && SDL_GL_SetSwapInterval(-1) != 0)
			SDL_GL_SetSwapInterval(0);
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		printf("glad could not load GL functions!\n");
		SDL_GL_DeleteContext(mGLContext);
		mGLContext = nullptr;
		return nullptr;
	}

	printf("GL %s on %s\n", (const char*)glGetString(GL_VERSION), (const char*)glGetString(GL_RENDERER));
	return mGLContext;
}

void Window::HandleEvent(SDL_Event& e)
{
	// Get mouse position
	SDL_GetMouseState(&mMouseX, &mMouseY);

	// Window event occured
	if (e.type == SDL_WINDOWEVENT)
	{
		switch (e.window.event)
		{
			// Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			if (mRenderer) SDL_RenderPresent(mRenderer);
			break;

			// Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			if (mRenderer) SDL_RenderPresent(mRenderer);
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

	if (mRenderer)
		SDL_RenderWindowToLogical(mRenderer, mMouseX, mMouseY, &mMouseLogicalPosition.X, &mMouseLogicalPosition.Y);
	else
		mMouseLogicalPosition = Vector2(static_cast<float>(mMouseX), static_cast<float>(mMouseY));
}

void Window::Free()
{
	// GL context before window
	if (mGLContext)
	{
		SDL_GL_DeleteContext(mGLContext);
		mGLContext = nullptr;
	}

	// Destory window and renderer
	SDL_DestroyWindow(mWindow);
	if (mRenderer) SDL_DestroyRenderer(mRenderer);

	// Deallocates internals
	mWindow = NULL;
	mRenderer = NULL;
}
