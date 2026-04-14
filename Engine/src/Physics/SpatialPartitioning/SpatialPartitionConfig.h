#pragma once
#include "../Vector2.h"

enum class SpatialPartitionStrategy
{
    Grid,
    QuadTree
};

struct SpatialPartitionConfig
{
    SpatialPartitionStrategy Strategy = SpatialPartitionStrategy::Grid;

    Vector2 WorldSize = Vector2(1920, 1080);

    float GridCellSize = 64.0f;

    int QuadTreeMaxObjects = 8;
    int QuadTreeMaxDepth = 5;

    bool SeparateStaticDynamic = true;
};
