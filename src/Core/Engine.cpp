#include "engine.h"
#include <iostream>
#include "../Physics/Vector2.h"

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

	//Load Foo' texture
	mTextureMap["Foo"] = new LTexture();
	if (!mTextureMap["Foo"]->LoadFromFile(Engine::GetInstance()->GetRenderer(), "res/foo.png"))
	{
		printf("Failed to load Foo' texture image!\n");
	}

	//Load background texture
	mTextureMap["Background"] = new LTexture();
	if (!mTextureMap["Background"]->LoadFromFile(Engine::GetInstance()->GetRenderer(), "res/moss2.png"))
	{
		printf("Failed to load background texture image!\n");
	}

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

	for (std::pair<std::string, LTexture*> var : mTextureMap)
	{
		var.second->Render(mRenderer, 0, 0);
	}

	SDL_RenderPresent(mRenderer);
}

void Engine::Events()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		Quit();
		break;
	}
}

bool Engine::Clean()
{
	for (std::pair<std::string, LTexture*> var : mTextureMap) {
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
