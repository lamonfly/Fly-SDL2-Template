#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <entt/entt.hpp>
#include "Window.h"
#include "../Scene/Scene.h"

class Engine
{
public:
	static Engine* GetInstance() { return (sInstance != nullptr)? sInstance : sInstance = new Engine(); }

	bool Init();
	template <typename T> void AddScene(const std::string& identifier) { mScenes[identifier] = []() { return new T(); }; };
	void LoadScene(const std::string&, const std::string&);
	void RemoveScene(const std::string&);
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
	std::map<std::string, std::function<Scene*()>> mScenes = {};
	std::map<std::string, Scene*> mActiveScenes = {};
	std::vector<std::function<void()>> tasks = {};

	Window* mWindow;
	static Engine* sInstance;

	Uint64 mNowUpdate = SDL_GetPerformanceCounter();
	Uint64 mLastUpdate = 0;
};

