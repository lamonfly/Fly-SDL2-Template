#pragma once
#include "../../Transform.h"
#include "../Collision.h"

struct Shape {
public:
	virtual Collision* Collide(Transform& current, Transform& other, Shape* shape) = 0;
};