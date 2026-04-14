#pragma once
#include "../../Transform.h"
#include "../Collision.h"
#include "../BoundingVolumes/AABB.h"
#include "../BoundingVolumes/BoundingSphere.h"

struct Shape {
public:
	virtual Collision* Collide(Transform& current, Transform& other, Shape* shape) = 0;

	virtual AABB GetAABB(const Transform& transform) const = 0;
	virtual BoundingSphere GetBoundingSphere(const Transform& transform) const = 0;
};