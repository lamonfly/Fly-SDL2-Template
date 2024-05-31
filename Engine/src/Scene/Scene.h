#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include "../Physics/Transform.h"

// Interface for scene setup
class Scene {
public:
	Scene() = default;
	~Scene();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void HandleEvent(SDL_Event& e) = 0;
protected:
	entt::registry mRegistry;

	template<typename T> inline void RenderType(SDL_Renderer* renderer) {
		for (auto&& [entity, transform, type] : mRegistry.view<Transform, T>().each()) {
			type.Render(renderer, transform);
		}
	}
};
