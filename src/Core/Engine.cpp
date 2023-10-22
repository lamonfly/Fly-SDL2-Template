#include "engine.h"
#include <iostream>
#include "../Physics/Transform.h"
#include "../Graphics/Sprite.h"

Engine* Engine::sInstance = nullptr;

bool Engine::Init()
{
	//Initialization flag
	mRunning = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
		mWindow = SDL_CreateWindow("Game Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
		if (mWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			mRunning = false;
		}
		else
		{
			//Create renderer for window
			mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (mRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				mRunning = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					mRunning = false;
				}
			}
		}
	}

	//Create components
	entt::entity foo = mRegistry.create();
	mRegistry.emplace<Transform>(foo);
	auto fooTex = new Texture();
	fooTex->LoadFromFile("res/foo.png", mRenderer);
	mTextureMap.emplace("foo", fooTex);
	mRegistry.emplace<Sprite>(foo, fooTex);

	entt::entity background = mRegistry.create();
	mRegistry.emplace<Transform>(background);
	auto backgroundTex = new Texture();
	backgroundTex->LoadFromFile("res/moss2.png", mRenderer);
	mTextureMap.emplace("foo", backgroundTex);
	mRegistry.emplace<Sprite>(background, backgroundTex);

	return mRunning;
}

void Engine::Update()
{

}

void Engine::Render()
{
	//Clear screen
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mRenderer);

	//Render sprite
	auto view = mRegistry.view<Transform, Sprite>();
	for (auto entity : view) 
	{
		auto[transform, sprite] = view.get(entity);

		sprite.Render(mRenderer, transform);
	}

	SDL_RenderPresent(mRenderer);
}

void Engine::Events()
{
	SDL_PollEvent(&mEvent);
	switch (mEvent.type)
	{
	case SDL_QUIT:
		Quit();
		break;
	}
}

bool Engine::Clean()
{
	for (std::pair<std::string, Texture*> var : mTextureMap) {
		var.second->Free();
	}
	mTextureMap.clear();

	//Destroy window	
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mWindow = NULL;
	mRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	return true;
}

void Engine::Quit()
{
 	mRunning = false;
}
