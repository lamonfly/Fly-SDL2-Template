#pragma once
#include "ISpatialPartition.h"
#include <unordered_map>
#include <algorithm>

class SpatialGrid : public ISpatialPartition
{
private:
    float mCellSize;
    Vector2 mWorldSize;
    std::unordered_map<int, std::vector<uint32_t>> mCells;
    int mObjectCount = 0;

    inline int Hash(int x, int y) const
    {
        return x * 73856093 ^ y * 19349663;
    }

    inline void GetCellCoords(const Vector2& point, int& outX, int& outY) const
    {
        outX = static_cast<int>(point.X / mCellSize);
        outY = static_cast<int>(point.Y / mCellSize);
    }

    void GetCellsForAABB(const AABB& bounds, std::vector<int>& outCells) const
    {
        int minX, minY, maxX, maxY;
        GetCellCoords(bounds.Min, minX, minY);
        GetCellCoords(bounds.Max, maxX, maxY);

        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                outCells.push_back(Hash(x, y));
            }
        }
    }

public:
    SpatialGrid(Vector2 worldSize, float cellSize)
        : mWorldSize(worldSize), mCellSize(cellSize)
    {
    }

    void Clear() override
    {
        mCells.clear();
        mObjectCount = 0;
    }

    void Insert(uint32_t id, const AABB& bounds) override
    {
        std::vector<int> cells;
        GetCellsForAABB(bounds, cells);

        for (int cellHash : cells)
        {
            mCells[cellHash].push_back(id);
        }
        mObjectCount++;
    }

    std::vector<uint32_t> Query(const AABB& bounds) const override
    {
        std::vector<uint32_t> results;
        std::vector<int> cells;
        GetCellsForAABB(bounds, cells);

        for (int cellHash : cells)
        {
            auto it = mCells.find(cellHash);
            if (it != mCells.end())
            {
                for (uint32_t id : it->second)
                {
                    if (std::find(results.begin(), results.end(), id) == results.end())
                    {
                        results.push_back(id);
                    }
                }
            }
        }

        return results;
    }

    int GetObjectCount() const override
    {
        return mObjectCount;
    }

    float GetCellSize() const { return mCellSize; }
    Vector2 GetWorldSize() const { return mWorldSize; }
};
