#pragma once
#include "Vector2.h"
#include "Collider/BoundingVolumes/AABB.h"

struct CCDResult
{
    bool Hit = false;
    float TimeOfImpact = 1.0f;
    Vector2 CollisionPoint;
    Vector2 CollisionNormal;

    CCDResult() = default;

    CCDResult(bool hit, float toi, Vector2 point, Vector2 normal)
        : Hit(hit), TimeOfImpact(toi), CollisionPoint(point), CollisionNormal(normal) {}
};

class CCD
{
public:
    static CCDResult SweepCircleCircle(
        Vector2 centerA, float radiusA, Vector2 velocityA,
        Vector2 centerB, float radiusB);

    static CCDResult SweepCircleAABB(
        Vector2 center, float radius, Vector2 velocity,
        const AABB& box);

    static CCDResult SweepAABBAABB(
        const AABB& boxA, Vector2 velocity,
        const AABB& boxB);

private:
    static bool SolveQuadratic(float a, float b, float c, float& t0, float& t1);

    static Vector2 ClosestPointOnAABB(const Vector2& point, const AABB& box);
};
