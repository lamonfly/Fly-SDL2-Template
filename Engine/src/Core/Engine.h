#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <vector>
#include <entt.hpp>

#include "Window.h"
#include "../Scene.h"

class Engine
{
public:
	static Engine* GetInstance() { return (sInstance != nullptr)? sInstance : sInstance = new Engine(); }

	bool Init(Scene*);
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
	std::vector<Scene*> mScenes = {};

	Window* mWindow;
	static Engine* sInstance;
};

