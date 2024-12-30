#include "CircleShape.h"
#include "RectShape.h"

Collision* CircleShape::Collide(Transform& current, Transform& other, Shape* shape) {
	if (auto* circle = dynamic_cast<CircleShape*>(shape)) {
		return ColliderTools::collideCircleCircle(current.Position, Radius, other.Position, circle->Radius);
	}
	else if (auto* rect = dynamic_cast<RectShape*>(shape)) {
		return ColliderTools::collideCircleRectangle(current.Position, Radius, other.Position, rect->Dimension);
	}

	throw std::logic_error("Collision function not implemented yet!");
}