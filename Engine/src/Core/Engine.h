#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <entt/entt.hpp>
#include "EngineConfig.h"
#include "Window.h"
#include "../Scene/Scene.h"

class Renderer3D;

class Engine
{
public:
	static Engine* GetInstance() { return (sInstance != nullptr)? sInstance : sInstance = new Engine(); }
	~Engine();

	bool Init(const EngineConfig& config = EngineConfig());
	template <typename T> void AddScene(const std::string& identifier) { mScenes[identifier] = []() { return new T(); }; };
	void LoadScene(const std::string&, const std::string&);
	void RemoveScene(const std::string&);
	bool Clean();
	void Quit();

	void Update();
	void Render();
	void Events();

	Window* GetWindow() { return mWindow; }
	Renderer3D* GetRenderer3D() { return mRenderer3D.get(); }
	const EngineConfig& GetConfig() const { return mConfig; }

	inline bool IsRunning() { return mRunning; }

private:
	Engine();
	bool mRunning;
	SDL_Event mEvent;
	EngineConfig mConfig;
	std::map<std::string, std::function<Scene*()>> mScenes = {};
	std::map<std::string, Scene*> mActiveScenes = {};
	std::vector<std::function<void()>> tasks = {};

	Window* mWindow;
	std::unique_ptr<Renderer3D> mRenderer3D;
	static Engine* sInstance;

	Uint64 mNowUpdate = SDL_GetPerformanceCounter();
	Uint64 mLastUpdate = 0;
};
