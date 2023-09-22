#pragma once
#include "LTexture.h"

class Sprite
{
private:
	LTexture* texture;

public:
	Sprite() = default;
	Sprite(const char* path)
	{
		texture = new LTexture();
		if (!texture->LoadFromFile(path))
		{
			printf("Failed to load %s texture image!\n", path);
		}
	}

	~Sprite()
	{
		texture->Free();
	}

	void Render(Transform transform)
	{
		texture->Render(transform.Position.X, transform.Position.Y);
	}
};