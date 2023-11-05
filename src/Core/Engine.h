#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <vector>
#include <entt.hpp>

#include "../Graphics/Texture.h"
#include "Window.h"

class Engine
{
public:
	static Engine* GetInstance() { return (sInstance != nullptr)? sInstance : sInstance = new Engine(); }

	bool Init();
	bool Clean();
	void Quit();

	void Update();
	void Render();
	void Events();

	Window* GetWindow() { return mWindow; }

	inline bool IsRunning() { return mRunning; }

private:
	Engine() {};
	bool mRunning;
	SDL_Event mEvent;
	entt::registry mRegistry;
	std::map<std::string, Texture*> mTextureMap;

	Window* mWindow;
	static Engine* sInstance;
};

