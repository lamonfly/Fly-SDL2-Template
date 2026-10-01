#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>
#include "RigidBody.h"
#include "CollisionLayer.h"

enum class ShapeType3D
{
	Box,
	Sphere,
	Capsule,
	Cylinder
};

// Needs Transform3. Position = center, meters
struct RigidBody3D
{
	static RigidBody3D Box(const glm::vec3& size, BodyType type)
	{
		RigidBody3D rb;
		rb.Shape = ShapeType3D::Box;
		rb.Type = type;
		rb.HalfExtents = size * 0.5f;
		return rb;
	}

	static RigidBody3D Sphere(float radius, BodyType type)
	{
		RigidBody3D rb;
		rb.Shape = ShapeType3D::Sphere;
		rb.Type = type;
		rb.Radius = radius;
		return rb;
	}

	// halfHeight = cylinder part only
	static RigidBody3D Capsule(float radius, float halfHeight, BodyType type)
	{
		RigidBody3D rb;
		rb.Shape = ShapeType3D::Capsule;
		rb.Type = type;
		rb.Radius = radius;
		rb.HalfHeight = halfHeight;
		return rb;
	}

	static RigidBody3D Cylinder(float radius, float halfHeight, BodyType type)
	{
		RigidBody3D rb;
		rb.Shape = ShapeType3D::Cylinder;
		rb.Type = type;
		rb.Radius = radius;
		rb.HalfHeight = halfHeight;
		return rb;
	}

	ShapeType3D Shape = ShapeType3D::Box;
	BodyType Type = BodyType::Static;
	glm::vec3 HalfExtents = glm::vec3(0.5f);
	float Radius = 0.5f;
	float HalfHeight = 0.5f;
	float Mass = 0.0f;  // 0 = Jolt density default

	CollisionMask Layer = CollisionLayer::Default;
	CollisionMask CollidesWith = CollisionLayer::All;

	float Restitution = 0.0f;
	float Friction = 0.2f;
	float LinearDamping = 0.05f;
	float AngularDamping = 0.05f;
	float GravityFactor = 1.0f;

	bool UseCCD = false;        // LinearCast motion quality
	bool IsSensor = false;      // contacts without response
	bool AllowSleeping = true;
	bool LockRotation = false;

	// Set by engine
	JPH::BodyID Id;
};
