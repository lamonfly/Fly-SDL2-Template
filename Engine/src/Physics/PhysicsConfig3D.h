#pragma once
#include <glm/glm.hpp>
#include <cstdint>

// Meters. Set before first RigidBody3D
struct PhysicsConfig3D
{
	// m/s^2, Y up
	glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

	float FixedTimeStep = 1.0f / 60.0f;
	int MaxSubSteps = 4;

	uint32_t MaxBodies = 1024;
	uint32_t MaxBodyPairs = 1024;
	uint32_t MaxContactConstraints = 1024;
};
