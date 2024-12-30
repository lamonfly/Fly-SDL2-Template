#pragma once
#include "../Vector2.h"

struct Collision {
public:
	Vector2 Point;

	Vector2 Normal;

	Collision(Vector2 point, Vector2 normal) : Point(point), Normal(normal) {};

	~Collision() = default;
};