#pragma once
#include "EntityComponentSystem/ECS.h"

struct TransformComponent : public Component
{
public:
	Vector2 Position;

	TransformComponent()
	{
		
	}

	TransformComponent(Vector2 vector)
	{
		Position = vector;
	}

	void init() override
	{

	}

	void update() override
	{
		
	}
};
