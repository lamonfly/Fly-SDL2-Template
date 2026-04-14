#pragma once
#include "../../Vector2.h"
#include <algorithm>

struct AABB
{
public:
    Vector2 Min;
    Vector2 Max;

    AABB() : Min(0, 0), Max(0, 0) {}
    AABB(Vector2 min, Vector2 max) : Min(min), Max(max) {}

    static AABB FromCenterAndExtents(Vector2 center, Vector2 extents)
    {
        return AABB(center - extents, center + extents);
    }

    static AABB FromPositionAndSize(Vector2 position, Vector2 size)
    {
        return AABB(position, position + size);
    }

    inline bool Intersects(const AABB& other) const
    {
        return !(Max.X < other.Min.X || Min.X > other.Max.X ||
                 Max.Y < other.Min.Y || Min.Y > other.Max.Y);
    }

    inline bool Contains(const Vector2& point) const
    {
        return point.X >= Min.X && point.X <= Max.X &&
               point.Y >= Min.Y && point.Y <= Max.Y;
    }

    inline bool Contains(const AABB& other) const
    {
        return other.Min.X >= Min.X && other.Max.X <= Max.X &&
               other.Min.Y >= Min.Y && other.Max.Y <= Max.Y;
    }

    static AABB Merge(const AABB& a, const AABB& b)
    {
        return AABB(
            Vector2(std::min(a.Min.X, b.Min.X), std::min(a.Min.Y, b.Min.Y)),
            Vector2(std::max(a.Max.X, b.Max.X), std::max(a.Max.Y, b.Max.Y))
        );
    }

    inline Vector2 GetCenter() const
    {
        return (Min + Max) * 0.5f;
    }

    inline Vector2 GetExtents() const
    {
        return (Max - Min) * 0.5f;
    }

    inline Vector2 GetSize() const
    {
        return Max - Min;
    }

    inline void Expand(float radius)
    {
        Min.X -= radius;
        Min.Y -= radius;
        Max.X += radius;
        Max.Y += radius;
    }

    inline void Expand(const Vector2& amount)
    {
        Min = Min - amount;
        Max = Max + amount;
    }

    inline float GetArea() const
    {
        Vector2 size = GetSize();
        return size.X * size.Y;
    }

    inline float GetPerimeter() const
    {
        Vector2 size = GetSize();
        return 2.0f * (size.X + size.Y);
    }
};
