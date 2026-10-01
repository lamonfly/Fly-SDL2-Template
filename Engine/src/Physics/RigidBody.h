#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include "Vector2.h"
#include "CollisionLayer.h"

enum class BodyType
{
	Static,     // never moves
	Kinematic,  // follows Transform
	Dynamic     // simulated, writes Transform
};

enum class ShapeType
{
	Box,
	Circle
};

// Needs Transform. Position = top-left
struct RigidBody
{
	static RigidBody Box(Vector2 sizePx, BodyType type)
	{
		RigidBody rb;
		rb.Shape = ShapeType::Box;
		rb.Type = type;
		rb.HalfExtentsPx = sizePx * 0.5f;
		return rb;
	}

	static RigidBody Circle(float radiusPx, BodyType type)
	{
		RigidBody rb;
		rb.Shape = ShapeType::Circle;
		rb.Type = type;
		rb.HalfExtentsPx = Vector2(radiusPx, radiusPx);
		return rb;
	}

	ShapeType Shape = ShapeType::Box;
	BodyType Type = BodyType::Static;
	Vector2 HalfExtentsPx = Vector2(0.5f, 0.5f);

	CollisionMask Layer = CollisionLayer::Default;
	CollisionMask CollidesWith = CollisionLayer::All;

	float Restitution = 0.0f;
	float Friction = 0.2f;
	float LinearDamping = 0.0f;
	float AngularDamping = 0.0f;
	float GravityFactor = 1.0f;

	bool UseCCD = false;        // LinearCast motion quality
	bool IsSensor = false;      // contacts without response
	bool AllowSleeping = true;
	bool LockRotation = false;

	// Set by engine
	JPH::BodyID Id;
};
