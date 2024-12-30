#pragma once
#include "Shape.h"
#include "../ColliderTools.h"
 
struct CircleShape : Shape
{
public:
	float Radius;
	CircleShape(float radius) : Radius(radius) {};

	Collision* Collide(Transform& current, Transform& other, Shape* shape) override;
};