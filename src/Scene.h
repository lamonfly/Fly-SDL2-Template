#pragma once
#include <entt.hpp>

// Interface for scene setup
class Scene {
public:
	Scene() = default;
	~Scene() = default;

	// Creates entites related to scene
	virtual bool Init(entt::registry* registry) = 0;
};