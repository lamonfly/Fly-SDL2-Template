#pragma once
#include "../SpatialPartitioning/ISpatialPartition.h"
#include "../SpatialPartitioning/SpatialGrid.h"
#include "../SpatialPartitioning/QuadTree.h"
#include "../SpatialPartitioning/SpatialPartitionConfig.h"
#include "Collider.h"
#include "../Transform.h"
#include <memory>
#include <vector>
#include <unordered_map>

struct CollisionPair
{
    uint32_t IdA;
    uint32_t IdB;
};

class BroadPhase
{
private:
    std::unique_ptr<ISpatialPartition> mPartition;
    std::unique_ptr<ISpatialPartition> mStaticPartition;
    SpatialPartitionConfig mConfig;

    std::unordered_map<uint32_t, AABB> mStaticAABBs;
    bool mStaticDirty = true;

    ISpatialPartition* CreatePartition(const SpatialPartitionConfig& config)
    {
        switch (config.Strategy)
        {
        case SpatialPartitionStrategy::Grid:
            return new SpatialGrid(config.WorldSize, config.GridCellSize);
        case SpatialPartitionStrategy::QuadTree:
            return new QuadTree(
                AABB(Vector2(0, 0), config.WorldSize),
                config.QuadTreeMaxObjects,
                config.QuadTreeMaxDepth
            );
        default:
            return new SpatialGrid(config.WorldSize, config.GridCellSize);
        }
    }

public:
    BroadPhase(const SpatialPartitionConfig& config = SpatialPartitionConfig())
        : mConfig(config)
    {
        mPartition.reset(CreatePartition(config));

        if (config.SeparateStaticDynamic)
        {
            mStaticPartition.reset(CreatePartition(config));
        }
    }

    void Update(
        const std::vector<uint32_t>& ids,
        const std::vector<Collider*>& colliders,
        const std::vector<Transform*>& transforms)
    {
        mPartition->Clear();

        if (mConfig.SeparateStaticDynamic && mStaticDirty)
        {
            mStaticPartition->Clear();
            mStaticAABBs.clear();
        }

        for (size_t i = 0; i < ids.size(); i++)
        {
            const AABB& aabb = colliders[i]->GetAABB(*transforms[i]);

            if (mConfig.SeparateStaticDynamic && colliders[i]->IsStatic)
            {
                if (mStaticDirty)
                {
                    mStaticPartition->Insert(ids[i], aabb);
                    mStaticAABBs[ids[i]] = aabb;
                }
            }
            else
            {
                mPartition->Insert(ids[i], aabb);
            }
        }

        if (mStaticDirty)
        {
            mStaticDirty = false;
        }
    }

    std::vector<CollisionPair> GetCollisionPairs(
        const std::vector<uint32_t>& ids,
        const std::vector<Collider*>& colliders,
        const std::vector<Transform*>& transforms)
    {
        std::vector<CollisionPair> pairs;
        std::unordered_map<uint64_t, bool> tested;

        for (size_t i = 0; i < ids.size(); i++)
        {
            const AABB& aabb = colliders[i]->GetAABB(*transforms[i]);
            std::vector<uint32_t> candidates;

            if (mConfig.SeparateStaticDynamic && !colliders[i]->IsStatic)
            {
                auto dynamicCandidates = mPartition->Query(aabb);
                candidates.insert(candidates.end(), dynamicCandidates.begin(), dynamicCandidates.end());

                auto staticCandidates = mStaticPartition->Query(aabb);
                candidates.insert(candidates.end(), staticCandidates.begin(), staticCandidates.end());
            }
            else if (!mConfig.SeparateStaticDynamic)
            {
                candidates = mPartition->Query(aabb);
            }

            for (uint32_t candidateId : candidates)
            {
                if (candidateId == ids[i])
                {
                    continue;
                }

                uint64_t pairKey = (static_cast<uint64_t>(std::min(ids[i], candidateId)) << 32) |
                    std::max(ids[i], candidateId);

                if (tested.find(pairKey) != tested.end())
                {
                    continue;
                }

                tested[pairKey] = true;

                size_t candidateIndex = SIZE_MAX;
                for (size_t j = 0; j < ids.size(); j++)
                {
                    if (ids[j] == candidateId)
                    {
                        candidateIndex = j;
                        break;
                    }
                }

                if (candidateIndex != SIZE_MAX)
                {
                    if (!colliders[i]->CanCollideWith(*colliders[candidateIndex]))
                    {
                        continue;
                    }

                    const AABB& candidateAABB = colliders[candidateIndex]->GetAABB(*transforms[candidateIndex]);

                    if (aabb.Intersects(candidateAABB))
                    {
                        pairs.push_back({ ids[i], candidateId });
                    }
                }
            }
        }

        return pairs;
    }

    void MarkStaticDirty()
    {
        mStaticDirty = true;
    }

    const SpatialPartitionConfig& GetConfig() const { return mConfig; }
    int GetDynamicObjectCount() const { return mPartition->GetObjectCount(); }
    int GetStaticObjectCount() const { return mStaticPartition ? mStaticPartition->GetObjectCount() : 0; }
};
