#pragma once

#include <Physics/Vector2.h>

class SnakePoint {
public:
	Vector2 Position;
	bool Ignore = false;

	SnakePoint(Vector2 positon) : Position(positon) {};
};