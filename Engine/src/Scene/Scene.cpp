#include "Scene.h"
#include "Camera.h"
#include "../Graphics/Sprite.h"

Scene::~Scene() {
	mRegistry.clear();
}

void Scene::Render(SDL_Renderer* renderer) {
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