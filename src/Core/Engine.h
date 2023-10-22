#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <vector>
#include <entt.hpp>

#include "../Graphics/Texture.h"

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

	inline bool IsRunning() { return mRunning; }
	inline SDL_Renderer* GetRenderer() { return mRenderer; }

private:
	Engine() {};
	bool mRunning;
	SDL_Event mEvent;
	entt::registry mRegistry;
	std::map<std::string, Texture*> mTextureMap;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	static Engine* sInstance;
};

