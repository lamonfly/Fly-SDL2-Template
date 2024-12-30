#pragma once
#include "../Vector2.h"
#include "Collision.h"

class ColliderTools
{
public:
	//Collision between circle circle
	static Collision* collideCircleCircle(Vector2 a, float aRadius, Vector2 b, float bRadius);
	//Collision between rectangle rectangle
	static Collision* collideRectangleRectangle(Vector2 a, Vector2 aDimension, Vector2 b, Vector2 bDimension);
	//Collision between circle rectangle
	static Collision* collideCircleRectangle(Vector2 a, float aRadius, Vector2 b, Vector2 bDimension);
};

