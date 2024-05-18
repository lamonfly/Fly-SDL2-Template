#include "Scene.h"
#include "Camera.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/Line.h"

Scene::~Scene() {
	mRegistry.clear();
}

void Scene::RenderSprite(SDL_Renderer* renderer) {
	// Render camera view
	auto spriteView = mRegistry.view<Transform, Sprite>();
	auto cameraView = mRegistry.view<Transform, Camera>();
	for (auto cameraEntity : cameraView)
	{
		auto [transform, camera] = cameraView.get(cameraEntity);

		for (auto spriteEntity : spriteView)
		{
			auto tuple = spriteView.get(spriteEntity);

			camera.RenderSprite(transform, tuple);
		}
	}
}

void Scene::RenderLine(SDL_Renderer* renderer) {
	// Render camera view
	auto lineView = mRegistry.view<Transform, Line>();
	auto cameraView = mRegistry.view<Transform, Camera>();
	for (auto cameraEntity : cameraView)
	{
		auto [transform, camera] = cameraView.get(cameraEntity);

		for (auto lineEntity : lineView)
		{
			auto tuple = lineView.get(lineEntity);

			std::get<Line&>(tuple).Render(renderer, std::get<Transform&>(tuple).Position + transform.Position);
		}
	}
}