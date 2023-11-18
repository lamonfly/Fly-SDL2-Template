#include "engine.h"
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

// TODO: Remove scene instantiation from engine init
#include "../SampleScene.h"

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
		mWindow = new Window();
		if (!mWindow->Init()) 
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			mRunning = false;
		}
		else
		{
			//Create renderer for window
			mWindow->CreateRenderer();
			if (mWindow->GetRenderer() == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				mRunning = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(mWindow->GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);

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

	mScenes.push_back(new SampleScene());

	return mRunning;
}

void Engine::Update()
{
	for (auto scene : mScenes)
	{
		scene->Update();
	}
}

void Engine::Render()
{
	if (mWindow->isMinimized())
		return;

	//Clear screen
	SDL_SetRenderDrawColor(mWindow->GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mWindow->GetRenderer());

	//Render scenes
	for (auto scene : mScenes) 
	{
		scene->Render(mWindow->GetRenderer());
	}

	SDL_RenderPresent(mWindow->GetRenderer());
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
		mWindow->HandleEvent(mEvent);

		for (auto scene : mScenes)
		{
			scene->HandleEvent(mEvent);
		}
	}
}

bool Engine::Clean()
{
	//Destroy window
	mWindow->Free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	return true;
}

void Engine::Quit()
{
 	mRunning = false;
}
