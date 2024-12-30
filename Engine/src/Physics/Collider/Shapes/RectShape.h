#pragma once
#include "Shape.h"
#include "../ColliderTools.h"

struct RectShape : Shape
{
public:
	Vector2 Dimension;

	RectShape(Vector2 dimension) : Dimension(dimension) {};

	Collision* Collide(Transform& current, Transform& other, Shape* shape) override;
};