#include "engine.h"
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "../Physics/JoltGlobals.h"
#include "../Graphics3D/Renderer3D.h"

Engine* Engine::sInstance = nullptr;

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::Init(const EngineConfig& config)
{
	//Initialization flag
	mRunning = true;
	mConfig = config;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		mRunning = false;
		return mRunning;
	}

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Create window
	mWindow = new Window();
	if (!mWindow->Init(config))
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		mRunning = false;
		return mRunning;
	}

	if (config.Backend == RenderBackend::OpenGL3)
	{
		//Create GL context and renderer
		if (mWindow->CreateGLContext() == nullptr)
		{
			mRunning = false;
			return mRunning;
		}

		mRenderer3D = std::make_unique<Renderer3D>();
		if (!mRenderer3D->Init())
		{
			printf("Renderer3D could not initialize!\n");
			mRunning = false;
			return mRunning;
		}
	}
	else
	{
		//Create renderer for window
		mWindow->CreateRenderer();
		if (mWindow->GetRenderer() == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			mRunning = false;
			return mRunning;
		}

		//Initialize renderer color
		SDL_SetRenderDrawColor(mWindow->GetRenderer(), mWindow->Color.r, mWindow->Color.g, mWindow->Color.b, mWindow->Color.a);
	}

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

	//Initialize Jolt Physics
	JoltGlobals::Init();

	return mRunning;
}

void Engine::LoadScene(const std::string& identifier, const std::string& activeId)
{
	tasks.push_back([identifier, activeId, this]() {
		auto it = mScenes.find(identifier);
		if (it != mScenes.end()) {
			Scene* newScene = it->second();
			newScene->Init();
			mActiveScenes[activeId] = newScene;
		}
	});
}

void Engine::RemoveScene(const std::string& activeId)
{
	tasks.push_back([activeId, this]() {
		auto it = mActiveScenes.find(activeId);
		if (it != mActiveScenes.end()) {
			delete it->second;
			mActiveScenes.erase(it);
		}
	});
}

void Engine::Update()
{
	if (mWindow->isMinimized()) {
		mNowUpdate = SDL_GetPerformanceCounter();
		return;
	}

	mLastUpdate = mNowUpdate;
	mNowUpdate = SDL_GetPerformanceCounter();
	double delta = (mNowUpdate - mLastUpdate) / (double)SDL_GetPerformanceFrequency();

	for (auto scene : mActiveScenes)
	{
		scene.second->UpdatePhysics(delta);
	}

	for (auto scene : mActiveScenes)
	{
		scene.second->Update(delta);
	}

	for (const auto& task : tasks) {
		task();
	}
	tasks.clear();
}

void Engine::Render()
{
	if (mWindow->isMinimized())
		return;

	if (mConfig.Backend == RenderBackend::OpenGL3)
	{
		int w = 0, h = 0;
		SDL_GL_GetDrawableSize(mWindow->GetSDLWindow(), &w, &h);
		if (w <= 0 || h <= 0)
			return;

		mRenderer3D->SetViewport(w, h);
		mRenderer3D->Clear(mWindow->Color);

		for (auto scene : mActiveScenes)
		{
			scene.second->Render3D(*mRenderer3D);
		}

		SDL_GL_SwapWindow(mWindow->GetSDLWindow());
		return;
	}

	//Clear screen
	SDL_SetRenderDrawColor(mWindow->GetRenderer(), mWindow->Color.r, mWindow->Color.g, mWindow->Color.b, mWindow->Color.a);
	SDL_RenderClear(mWindow->GetRenderer());

	//Set scale
	SDL_RenderSetScale(mWindow->GetRenderer(), (float)mWindow->GetWidth() / mWindow->GetResolutionWidth(), (float)mWindow->GetHeight() / mWindow->GetResolutionHeight());

	//Render scenes
	for (auto scene : mActiveScenes)
	{
		scene.second->Render(mWindow->GetRenderer());
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

		for (auto scene : mActiveScenes)
		{
			scene.second->HandleEvent(mEvent);
		}
	}
}

bool Engine::Clean()
{
	//Destroy scenes before Jolt and GL
	for (auto& scene : mActiveScenes) {
		delete scene.second;
	}
	mActiveScenes.clear();

	//GL objects before context
	mRenderer3D.reset();

	JoltGlobals::Shutdown();

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
