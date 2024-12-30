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