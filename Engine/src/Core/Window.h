#pragma once

#include <SDL.h>
#include "EngineConfig.h"
#include "../Physics/Vector2.h"

class Window
{
public:
	// Initializes internals
	Window();

	// Creates window. GL attributes set here when backend is OpenGL3
	bool Init(const EngineConfig& config);

	// Creates renderer from internal window. SDL2D only
	SDL_Renderer* CreateRenderer();

	// Creates GL context and loads glad. OpenGL3 only
	SDL_GLContext CreateGLContext();

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

	// Window renderer, null in OpenGL3
	inline SDL_Renderer* GetRenderer() { return mRenderer; }

	inline SDL_Window* GetSDLWindow() { return mWindow; }
	inline SDL_GLContext GetGLContext() { return mGLContext; }
	inline RenderBackend GetBackend() { return mBackend; }

	// Window focus
	bool isMinimized() { return mMinimized; }

	// Window Background Color
	SDL_Color Color = { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF };

	// Mouse Position. Logical in SDL2D, window pixels in OpenGL3
	inline Vector2 GetMouseLogicalPosition() { return mMouseLogicalPosition; }

private:
	// Window data
	SDL_Window* mWindow;

	// Window renderer
	SDL_Renderer* mRenderer;

	// GL context
	SDL_GLContext mGLContext;
	RenderBackend mBackend;
	EngineConfig mConfig;

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
