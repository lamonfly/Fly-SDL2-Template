#pragma once
#include "ISpatialPartition.h"
#include <memory>
#include <algorithm>

class QuadTree : public ISpatialPartition
{
private:
    struct Node
    {
        AABB Bounds;
        std::vector<std::pair<uint32_t, AABB>> Objects;
        std::unique_ptr<Node> Children[4];
        int Depth;

        Node(const AABB& bounds, int depth)
            : Bounds(bounds), Depth(depth)
        {
        }

        bool IsLeaf() const
        {
            return Children[0] == nullptr;
        }

        void Subdivide()
        {
            Vector2 center = Bounds.GetCenter();
            Vector2 extent = Bounds.GetExtents();

            Children[0] = std::make_unique<Node>(
                AABB(Bounds.Min, center), Depth + 1);
            Children[1] = std::make_unique<Node>(
                AABB(Vector2(center.X, Bounds.Min.Y), Vector2(Bounds.Max.X, center.Y)), Depth + 1);
            Children[2] = std::make_unique<Node>(
                AABB(Vector2(Bounds.Min.X, center.Y), Vector2(center.X, Bounds.Max.Y)), Depth + 1);
            Children[3] = std::make_unique<Node>(
                AABB(center, Bounds.Max), Depth + 1);
        }

        bool Insert(uint32_t id, const AABB& objectBounds, int maxObjects, int maxDepth)
        {
            if (!Bounds.Intersects(objectBounds))
            {
                return false;
            }

            if (IsLeaf() && (Objects.size() < maxObjects || Depth >= maxDepth))
            {
                Objects.push_back({ id, objectBounds });
                return true;
            }

            if (IsLeaf())
            {
                Subdivide();

                std::vector<std::pair<uint32_t, AABB>> oldObjects = Objects;
                Objects.clear();

                for (const auto& obj : oldObjects)
                {
                    bool inserted = false;
                    for (int i = 0; i < 4; i++)
                    {
                        if (Children[i]->Bounds.Contains(obj.second))
                        {
                            Children[i]->Insert(obj.first, obj.second, maxObjects, maxDepth);
                            inserted = true;
                            break;
                        }
                    }

                    if (!inserted)
                    {
                        Objects.push_back(obj);
                    }
                }
            }

            for (int i = 0; i < 4; i++)
            {
                if (Children[i]->Bounds.Contains(objectBounds))
                {
                    return Children[i]->Insert(id, objectBounds, maxObjects, maxDepth);
                }
            }

            Objects.push_back({ id, objectBounds });
            return true;
        }

        void Query(const AABB& bounds, std::vector<uint32_t>& results) const
        {
            if (!Bounds.Intersects(bounds))
            {
                return;
            }

            for (const auto& obj : Objects)
            {
                if (obj.second.Intersects(bounds))
                {
                    if (std::find(results.begin(), results.end(), obj.first) == results.end())
                    {
                        results.push_back(obj.first);
                    }
                }
            }

            if (!IsLeaf())
            {
                for (int i = 0; i < 4; i++)
                {
                    Children[i]->Query(bounds, results);
                }
            }
        }

        void Clear()
        {
            Objects.clear();
            for (int i = 0; i < 4; i++)
            {
                Children[i].reset();
            }
        }
    };

    std::unique_ptr<Node> mRoot;
    int mMaxObjects;
    int mMaxDepth;
    int mObjectCount = 0;

public:
    QuadTree(const AABB& bounds, int maxObjects = 8, int maxDepth = 5)
        : mMaxObjects(maxObjects), mMaxDepth(maxDepth), mObjectCount(0)
    {
        mRoot = std::make_unique<Node>(bounds, 0);
    }

    void Clear() override
    {
        if (mRoot)
        {
            mRoot->Clear();
        }
        mObjectCount = 0;
    }

    void Insert(uint32_t id, const AABB& bounds) override
    {
        if (mRoot->Insert(id, bounds, mMaxObjects, mMaxDepth))
        {
            mObjectCount++;
        }
    }

    std::vector<uint32_t> Query(const AABB& bounds) const override
    {
        std::vector<uint32_t> results;
        if (mRoot)
        {
            mRoot->Query(bounds, results);
        }
        return results;
    }

    int GetObjectCount() const override
    {
        return mObjectCount;
    }

    int GetMaxObjects() const { return mMaxObjects; }
    int GetMaxDepth() const { return mMaxDepth; }
};
