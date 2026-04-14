#pragma once
#include "Vector2.h"

struct Velocity
{
    Vector2 Value;

    Velocity() : Value(0, 0) {}
    Velocity(Vector2 velocity) : Value(velocity) {}
    Velocity(float x, float y) : Value(x, y) {}
};
