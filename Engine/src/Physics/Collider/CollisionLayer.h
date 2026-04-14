#pragma once
#include <cstdint>

enum class CollisionLayer : uint16_t
{
    Default    = 1 << 0,
    Player     = 1 << 1,
    Enemy      = 1 << 2,
    Projectile = 1 << 3,
    World      = 1 << 4,
    Trigger    = 1 << 5,
    PowerUp    = 1 << 6,
    Debris     = 1 << 7,

    All = 0xFFFF
};

inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b)
{
    return static_cast<CollisionLayer>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline CollisionLayer operator&(CollisionLayer a, CollisionLayer b)
{
    return static_cast<CollisionLayer>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

inline uint16_t operator~(CollisionLayer layer)
{
    return ~static_cast<uint16_t>(layer);
}

class CollisionMatrix
{
private:
    uint16_t mMatrix[16];

public:
    CollisionMatrix()
    {
        for (int i = 0; i < 16; i++)
        {
            mMatrix[i] = 0xFFFF;
        }
    }

    void SetLayerCollision(CollisionLayer layerA, CollisionLayer layerB, bool enabled)
    {
        int indexA = GetLayerIndex(layerA);
        int indexB = GetLayerIndex(layerB);

        if (indexA < 0 || indexB < 0) return;

        uint16_t mask = static_cast<uint16_t>(layerB);

        if (enabled)
        {
            mMatrix[indexA] |= mask;
            mMatrix[indexB] |= static_cast<uint16_t>(layerA);
        }
        else
        {
            mMatrix[indexA] &= ~mask;
            mMatrix[indexB] &= ~static_cast<uint16_t>(layerA);
        }
    }

    bool ShouldCollide(CollisionLayer layerA, CollisionLayer layerB) const
    {
        int indexA = GetLayerIndex(layerA);
        if (indexA < 0) return true;

        return (mMatrix[indexA] & static_cast<uint16_t>(layerB)) != 0;
    }

    bool ShouldCollide(CollisionLayer layer, uint16_t collidesWith) const
    {
        return (static_cast<uint16_t>(layer) & collidesWith) != 0;
    }

private:
    int GetLayerIndex(CollisionLayer layer) const
    {
        uint16_t value = static_cast<uint16_t>(layer);

        for (int i = 0; i < 16; i++)
        {
            if (value == (1 << i))
            {
                return i;
            }
        }

        return -1;
    }
};
