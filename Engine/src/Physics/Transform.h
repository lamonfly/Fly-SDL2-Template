#pragma once
#include "Vector2.h"

struct Transform
{
public:
	int scale = 1;
	Vector2 Size = Vector2(32, 32);
	Vector2 Position;
	double Rotation = 0.0f;

	Transform() = default;

	Transform(Vector2 vector)
	{
		Position = vector;
	}
};
