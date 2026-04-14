#include "RectShape.h"
#include "CircleShape.h"

Collision* RectShape::Collide(Transform& current, Transform& other, Shape* shape) {
	if (auto* rect = dynamic_cast<RectShape*>(shape)) {
		return ColliderTools::collideRectangleRectangle(current.Position, Dimension, other.Position, rect->Dimension);
	}
	else if (auto* circle = dynamic_cast<CircleShape*>(shape)) {
		return ColliderTools::collideCircleRectangle(other.Position, circle->Radius, current.Position, Dimension);
	}

	throw std::logic_error("Collision function not implemented yet!");
}

AABB RectShape::GetAABB(const Transform& transform) const
{
	Vector2 scaledDimension = Dimension * transform.Scale;
	return AABB(transform.Position, transform.Position + scaledDimension);
}

BoundingSphere RectShape::GetBoundingSphere(const Transform& transform) const
{
	Vector2 scaledDimension = Dimension * transform.Scale;
	Vector2 center = transform.Position + scaledDimension * 0.5f;
	float radius = scaledDimension.Magnitude() * 0.5f;
	return BoundingSphere(center, radius);
}