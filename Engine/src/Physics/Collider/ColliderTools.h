#pragma once
#include "../Vector2.h"
#include "Collision.h"
#include "BoundingVolumes/AABB.h"
#include "BoundingVolumes/BoundingSphere.h"

class ColliderTools
{
public:
	static bool TestAABB(const AABB& a, const AABB& b);

	static bool TestSphere(const BoundingSphere& a, const BoundingSphere& b);

	static Collision* collideCircleCircle(Vector2 a, float aRadius, Vector2 b, float bRadius);

	static Collision* collideRectangleRectangle(Vector2 a, Vector2 aDimension, Vector2 b, Vector2 bDimension);

	static Collision* collideCircleRectangle(Vector2 a, float aRadius, Vector2 b, Vector2 bDimension);
};

