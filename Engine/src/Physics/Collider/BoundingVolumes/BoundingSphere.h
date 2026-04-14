#pragma once
#include "../../Vector2.h"
#include "AABB.h"
#include <algorithm>

struct BoundingSphere
{
public:
    Vector2 Center;
    float Radius;

    BoundingSphere() : Center(0, 0), Radius(0) {}
    BoundingSphere(Vector2 center, float radius) : Center(center), Radius(radius) {}

    inline bool Intersects(const BoundingSphere& other) const
    {
        float distance = Center.Distance(other.Center);
        return distance < (Radius + other.Radius);
    }

    inline bool Intersects(const AABB& box) const
    {
        float closestX = std::max(box.Min.X, std::min(Center.X, box.Max.X));
        float closestY = std::max(box.Min.Y, std::min(Center.Y, box.Max.Y));

        Vector2 closest(closestX, closestY);
        float distance = Center.Distance(closest);

        return distance <= Radius;
    }

    inline bool Contains(const Vector2& point) const
    {
        return Center.Distance(point) <= Radius;
    }

    inline bool Contains(const BoundingSphere& other) const
    {
        float distance = Center.Distance(other.Center);
        return distance + other.Radius <= Radius;
    }

    static BoundingSphere Merge(const BoundingSphere& a, const BoundingSphere& b)
    {
        Vector2 diff = b.Center - a.Center;
        float distance = diff.Magnitude();

        if (distance + b.Radius <= a.Radius)
        {
            return a;
        }
        if (distance + a.Radius <= b.Radius)
        {
            return b;
        }

        float newRadius = (distance + a.Radius + b.Radius) * 0.5f;
        Vector2 newCenter = a.Center + diff.Normalized() * (newRadius - a.Radius);

        return BoundingSphere(newCenter, newRadius);
    }

    inline AABB GetAABB() const
    {
        return AABB(
            Vector2(Center.X - Radius, Center.Y - Radius),
            Vector2(Center.X + Radius, Center.Y + Radius)
        );
    }

    inline void Expand(float amount)
    {
        Radius += amount;
    }
};
