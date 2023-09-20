#pragma once
#include "EntityComponentSystem/ECS.h"
#include "EntityComponentSystem/Components.h"
#include "Graphics/LTexture.h"

class SpriteComponent : public Component 
{
private:
	TransformComponent* transform;
	LTexture* texture;

public:
	SpriteComponent() = default;
	SpriteComponent(const char* path) 
	{
		texture = new LTexture();
		if (!texture->LoadFromFile(path))
		{
			printf("Failed to load %s texture image!\n", path);
		}
	}

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
	}

	void update() override
	{

	}

	void draw() override
	{
		texture->Render(transform->Position.X, transform->Position.Y);
	}
};