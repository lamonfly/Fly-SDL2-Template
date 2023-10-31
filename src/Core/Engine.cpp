#include "engine.h"
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "../Physics/Transform.h"
#include "../Graphics/Sprite.h"

Engine* Engine::sInstance = nullptr;

bool Engine::Init()
{
	//Initialization flag
	mRunning = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		mRunning = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		if (!mWindow.Init()) 
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			mRunning = false;
		}
		else
		{
			//Create renderer for window
			mWindow.CreateRenderer();
			if (mWindow.GetRenderer() == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				mRunning = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(mWindow.GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					mRunning = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1) 
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					mRunning = false;
				}

				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					mRunning = false;
				}
			}
		}
	}

	//Create components
	entt::entity foo = mRegistry.create();
	mRegistry.emplace<Transform>(foo);
	auto fooTex = new Texture();
	fooTex->LoadFromFile(mWindow.GetRenderer(), "res/foo.png");
	mTextureMap.emplace("foo", fooTex);
	mRegistry.emplace<Sprite>(foo, fooTex);

	entt::entity background = mRegistry.create();
	mRegistry.emplace<Transform>(background);
	auto backgroundTex = new Texture();
	backgroundTex->LoadFromFile(mWindow.GetRenderer(), "res/moss2.png");
	mTextureMap.emplace("foo", backgroundTex);
	mRegistry.emplace<Sprite>(background, backgroundTex);

	return mRunning;
}

void Engine::Update()
{

}

void Engine::Render()
{
	if (mWindow.isMinimized())
		return;

	//Clear screen
	SDL_SetRenderDrawColor(mWindow.GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mWindow.GetRenderer());

	//Render sprite
	auto view = mRegistry.view<Transform, Sprite>();
	for (auto entity : view) 
	{
		auto[transform, sprite] = view.get(entity);

		sprite.Render(mWindow.GetRenderer(), transform);
	}

	SDL_RenderPresent(mWindow.GetRenderer());
}

void Engine::Events()
{
	// Handle event on queue
	while (SDL_PollEvent(&mEvent) != 0) {
		switch (mEvent.type)
		{
		case SDL_QUIT:
			Quit();
			break;
		}

		// Handle window events
		mWindow.HandleEvent(mEvent);
	}
}

bool Engine::Clean()
{
	for (std::pair<std::string, Texture*> var : mTextureMap) {
		var.second->Free();
	}
	mTextureMap.clear();

	//Destroy window
	mWindow.Free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	return true;
}

void Engine::Quit()
{
 	mRunning = false;
}
