#include "CCD.h"
#include <algorithm>
#include <cmath>

bool CCD::SolveQuadratic(float a, float b, float c, float& t0, float& t1)
{
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }

    float sqrtDisc = std::sqrt(discriminant);
    float invA = 1.0f / (2.0f * a);

    t0 = (-b - sqrtDisc) * invA;
    t1 = (-b + sqrtDisc) * invA;

    if (t0 > t1)
    {
        std::swap(t0, t1);
    }

    return true;
}

CCDResult CCD::SweepCircleCircle(
    Vector2 centerA, float radiusA, Vector2 velocityA,
    Vector2 centerB, float radiusB)
{
    Vector2 relativePos = centerA - centerB;
    float combinedRadius = radiusA + radiusB;

    if (velocityA.Magnitude() < 0.001f)
    {
        float distance = relativePos.Magnitude();
        if (distance < combinedRadius)
        {
            Vector2 normal = relativePos.Normalized();
            return CCDResult(true, 0.0f, centerB + normal * radiusB, normal);
        }
        return CCDResult();
    }

    float a = velocityA.X * velocityA.X + velocityA.Y * velocityA.Y;
    float b = 2.0f * (relativePos.X * velocityA.X + relativePos.Y * velocityA.Y);
    float c = relativePos.X * relativePos.X + relativePos.Y * relativePos.Y - combinedRadius * combinedRadius;

    float t0, t1;
    if (!SolveQuadratic(a, b, c, t0, t1))
    {
        return CCDResult();
    }

    float toi = t0;
    if (toi < 0.0f)
    {
        toi = t1;
    }

    if (toi < 0.0f || toi > 1.0f)
    {
        return CCDResult();
    }

    Vector2 collisionPos = centerA + velocityA * toi;
    Vector2 normal = (collisionPos - centerB).Normalized();
    Vector2 collisionPoint = centerB + normal * radiusB;

    return CCDResult(true, toi, collisionPoint, normal);
}

CCDResult CCD::SweepCircleAABB(
    Vector2 center, float radius, Vector2 velocity,
    const AABB& box)
{
    if (velocity.Magnitude() < 0.001f)
    {
        Vector2 closest = ClosestPointOnAABB(center, box);
        float distance = center.Distance(closest);

        if (distance < radius)
        {
            Vector2 normal = (center - closest).Normalized();
            return CCDResult(true, 0.0f, closest, normal);
        }
        return CCDResult();
    }

    AABB expandedBox = box;
    expandedBox.Expand(radius);

    Vector2 invVelocity;
    invVelocity.X = (velocity.X != 0.0f) ? 1.0f / velocity.X : FLT_MAX;
    invVelocity.Y = (velocity.Y != 0.0f) ? 1.0f / velocity.Y : FLT_MAX;

    float tmin = (expandedBox.Min.X - center.X) * invVelocity.X;
    float tmax = (expandedBox.Max.X - center.X) * invVelocity.X;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (expandedBox.Min.Y - center.Y) * invVelocity.Y;
    float tymax = (expandedBox.Max.Y - center.Y) * invVelocity.Y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
    {
        return CCDResult();
    }

    tmin = std::max(tmin, tymin);
    tmax = std::min(tmax, tymax);

    if (tmin < 0.0f)
    {
        tmin = tmax;
    }

    if (tmin < 0.0f || tmin > 1.0f)
    {
        return CCDResult();
    }

    Vector2 collisionPos = center + velocity * tmin;
    Vector2 closest = ClosestPointOnAABB(collisionPos, box);
    Vector2 normal = (collisionPos - closest).Normalized();

    return CCDResult(true, tmin, closest, normal);
}

CCDResult CCD::SweepAABBAABB(
    const AABB& boxA, Vector2 velocity,
    const AABB& boxB)
{
    if (velocity.Magnitude() < 0.001f)
    {
        if (boxA.Intersects(boxB))
        {
            Vector2 centerA = boxA.GetCenter();
            Vector2 centerB = boxB.GetCenter();
            Vector2 normal = (centerA - centerB).Normalized();
            return CCDResult(true, 0.0f, centerB, normal);
        }
        return CCDResult();
    }

    Vector2 invVelocity;
    invVelocity.X = (velocity.X != 0.0f) ? 1.0f / velocity.X : FLT_MAX;
    invVelocity.Y = (velocity.Y != 0.0f) ? 1.0f / velocity.Y : FLT_MAX;

    float tmin = (boxB.Min.X - boxA.Max.X) * invVelocity.X;
    float tmax = (boxB.Max.X - boxA.Min.X) * invVelocity.X;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (boxB.Min.Y - boxA.Max.Y) * invVelocity.Y;
    float tymax = (boxB.Max.Y - boxA.Min.Y) * invVelocity.Y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
    {
        return CCDResult();
    }

    tmin = std::max(tmin, tymin);
    tmax = std::min(tmax, tymax);

    if (tmin < 0.0f)
    {
        tmin = tmax;
    }

    if (tmin < 0.0f || tmin > 1.0f)
    {
        return CCDResult();
    }

    Vector2 normal;
    if (std::abs(velocity.X) > std::abs(velocity.Y))
    {
        normal = (velocity.X > 0) ? Vector2(-1, 0) : Vector2(1, 0);
    }
    else
    {
        normal = (velocity.Y > 0) ? Vector2(0, -1) : Vector2(0, 1);
    }

    Vector2 collisionPoint = boxA.GetCenter() + velocity * tmin;

    return CCDResult(true, tmin, collisionPoint, normal);
}

Vector2 CCD::ClosestPointOnAABB(const Vector2& point, const AABB& box)
{
    return Vector2(
        std::max(box.Min.X, std::min(point.X, box.Max.X)),
        std::max(box.Min.Y, std::min(point.Y, box.Max.Y))
    );
}
