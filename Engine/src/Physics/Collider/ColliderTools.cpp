#include "ColliderTools.h"
#include <iostream>

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
	// Rectangle A
    auto rectABottomRight = Vector2(a.X + aDimension.X, a.Y + aDimension.Y);

    // Rectangle B
    auto rectBBottomRight = Vector2(b.X + bDimension.X, b.Y + bDimension.Y);

    // Check if rectangles overlap
    if (rectABottomRight.X < b.X &&
        a.X > rectBBottomRight.X &&
        rectABottomRight.Y < b.Y &&
        a.Y > rectBBottomRight.Y) 
    {
        // Point of collision: the center of the overlap area
        Vector2 collisionPoint = Vector2((rectABottomRight.X + a.X) / 2.0f, (rectABottomRight.Y + a.Y) / 2.0f);
        // Normal vector (simple approach for overlap)
        Vector2 normal = Vector2(rectBBottomRight.X - a.X, rectABottomRight.Y - a.Y);  // Can be improved based on overlap side
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