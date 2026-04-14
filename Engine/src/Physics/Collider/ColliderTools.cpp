#include "ColliderTools.h"
#include <iostream>

bool ColliderTools::TestAABB(const AABB& a, const AABB& b)
{
    return a.Intersects(b);
}

bool ColliderTools::TestSphere(const BoundingSphere& a, const BoundingSphere& b)
{
    return a.Intersects(b);
}

Collision* ColliderTools::collideCircleCircle(Vector2 a, float aRadius, Vector2 b, float bRadius)
{
    // Calculate the distance between the centers
    Vector2 diff = a - b;
    float distance = diff.Magnitude();

    // Check if the circles are colliding
    if (distance < aRadius + bRadius) {
        // The point of collision is the midpoint between the centers of the two circles
        Vector2 collisionPoint = (a + b) * 0.5f;
        // The normal vector is the unit vector pointing from one circle to the other
        Vector2 normal = diff.Normalized();
        return new Collision(collisionPoint, normal);
    }
    return NULL;
}

Collision* ColliderTools::collideRectangleRectangle(Vector2 a, Vector2 aDimension, Vector2 b, Vector2 bDimension)
{
	auto rectABottomRight = Vector2(a.X + aDimension.X, a.Y + aDimension.Y);
	auto rectBBottomRight = Vector2(b.X + bDimension.X, b.Y + bDimension.Y);

	if (a.X < rectBBottomRight.X &&
		rectABottomRight.X > b.X &&
		a.Y < rectBBottomRight.Y &&
		rectABottomRight.Y > b.Y) 
	{
		float overlapX = std::min(rectABottomRight.X, rectBBottomRight.X) - std::max(a.X, b.X);
		float overlapY = std::min(rectABottomRight.Y, rectBBottomRight.Y) - std::max(a.Y, b.Y);

		Vector2 collisionPoint = Vector2(
			std::max(a.X, b.X) + overlapX * 0.5f,
			std::max(a.Y, b.Y) + overlapY * 0.5f
		);

		Vector2 normal;
		if (overlapX < overlapY) {
			normal = (a.X < b.X) ? Vector2(-1, 0) : Vector2(1, 0);
		} else {
			normal = (a.Y < b.Y) ? Vector2(0, -1) : Vector2(0, 1);
		}

		return new Collision(collisionPoint, normal);
	}
	return NULL;
}

Collision* ColliderTools::collideCircleRectangle(Vector2 a, float aRadius, Vector2 b, Vector2 bDimension)
{
    auto rectBottomRight = Vector2(b.X + bDimension.X, b.Y + bDimension.Y);
    auto circleCenter = Vector2(a.X + aRadius, a.Y + aRadius);

    // Find the closest point on the rectangle to the circle's center
    float closestX = std::fmax(b.X, std::fmin(circleCenter.X, rectBottomRight.X));
    float closestY = std::fmax(b.Y, std::fmin(circleCenter.Y, rectBottomRight.Y));

    // Vector from the closest point on the rectangle to the circle center
    float distance = circleCenter.Distance(Vector2(closestX, closestY));

    // Check if the circle collides with the rectangle
    if (distance <= aRadius) {
        // The collision point is the closest point on the rectangle
        Vector2 collisionPoint = Vector2(closestX, closestY);

        // Normal vector: the unit vector pointing from the circle center to the closest point
        Vector2 normal;
        if (distance <= 0.00f) {
            // Inside the rectangle, return the normal to the nearest edge
            float closestEdgeDistance = std::min({ std::fabs(circleCenter.X - b.X), std::fabs(circleCenter.X - rectBottomRight.X),
                                                   std::fabs(circleCenter.Y - b.Y), std::fabs(circleCenter.Y - rectBottomRight.Y) });

            // Find the direction to the nearest edge
            if (closestEdgeDistance == std::fabs(circleCenter.X - b.X)) {
                normal = { -1, 0 };  // Left edge
            }
            else if (closestEdgeDistance == std::fabs(circleCenter.X - rectBottomRight.X)) {
                normal = { 1, 0 };   // Right edge
            }
            else if (closestEdgeDistance == std::fabs(circleCenter.Y - b.Y)) {
                normal = { 0, -1 };  // Bottom edge
            }
            else {
                normal = { 0, 1 };   // Top edge
            }
        }
        else {
            normal = (collisionPoint - circleCenter).Normalized();
        }

        return new Collision(collisionPoint, normal);
    }
    return NULL;
}