#pragma once
#include "Vector2.h"
#include <cstdint>

// Set before first RigidBody
struct PhysicsConfig
{
	float PixelsPerMeter = 32.0f;

	// m/s^2, Y down
	Vector2 Gravity = Vector2(0.0f, 9.81f);

	float FixedTimeStep = 1.0f / 60.0f;
	int MaxSubSteps = 4;

	uint32_t MaxBodies = 1024;
	uint32_t MaxBodyPairs = 1024;
	uint32_t MaxContactConstraints = 1024;
};
