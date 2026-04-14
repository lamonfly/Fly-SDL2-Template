#pragma once
#include <vector>
#include <cstdint>
#include "../Collider/BoundingVolumes/AABB.h"

class ISpatialPartition
{
public:
    virtual ~ISpatialPartition() = default;

    virtual void Clear() = 0;

    virtual void Insert(uint32_t id, const AABB& bounds) = 0;

    virtual std::vector<uint32_t> Query(const AABB& bounds) const = 0;

    virtual int GetObjectCount() const = 0;
};
