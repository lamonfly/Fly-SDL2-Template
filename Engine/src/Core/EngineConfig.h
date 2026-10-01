#pragma once

enum class RenderBackend
{
	SDL2D,    // SDL_Renderer, Scene::Render
	OpenGL3   // GL 3.3 core, Scene::Render3D
};

// One backend per process
struct EngineConfig
{
	RenderBackend Backend = RenderBackend::SDL2D;
	bool VSync = true;
	int MSAASamples = 0;   // 0 = off
	int DepthBits = 24;
};
