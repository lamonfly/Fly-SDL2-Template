#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include "../Physics/Transform.h"

// Interface for scene setup
class Scene {
public:
	virtual ~Scene();

	virtual void Init() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void HandleEvent(SDL_Event& e) = 0;
	void UpdatePhysics(double deltaTime);
protected:
	entt::registry mRegistry;

	template<typename T> inline void RenderType(SDL_Renderer* renderer) {
		for (auto&& [entity, transform, type] : mRegistry.view<Transform, T>().each()) {
			type.Render(renderer, transform);
		}
	}

	template<typename T> inline void UpdateType(double deltaTime) {
		for (auto&& [entity, type] : mRegistry.view<T>().each()) {
			type.Update(deltaTime);
		}
	}

	template<typename T> inline void HandleEventType(SDL_Event& currentEvent) {
		for (auto&& [entity, type] : mRegistry.view<T>().each()) {
			type.HandleEvent(currentEvent);
		}
	}
};
