#include "PhysicsWorld.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

static constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
static constexpr float kRadToDeg = 180.0f / 3.14159265358979323846f;

void PhysicsWorld::Init(const PhysicsConfig& config)
{
	mConfig = config;

	JoltWorld::Capacities capacities;
	capacities.MaxBodies = config.MaxBodies;
	capacities.MaxBodyPairs = config.MaxBodyPairs;
	capacities.MaxContactConstraints = config.MaxContactConstraints;

	JoltWorld::Stepping stepping;
	stepping.FixedTimeStep = config.FixedTimeStep;
	stepping.MaxSubSteps = config.MaxSubSteps;

	mJolt.Init(capacities, stepping, JPH::Vec3(config.Gravity.X, config.Gravity.Y, 0.0f));
}

JPH::RVec3 PhysicsWorld::CenterFromTransform(Transform& transform, const RigidBody& body) const
{
	return ToMeters(transform.Position + body.HalfExtentsPx);
}

JPH::Quat PhysicsWorld::RotationFromTransform(Transform& transform) const
{
	return JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), static_cast<float>(transform.getRotation()) * kDegToRad);
}

void PhysicsWorld::CreateBody(entt::entity entity, Transform& transform, RigidBody& body)
{
	if (!mJolt.IsInitialized()) Init(mConfig);

	JPH::RefConst<JPH::Shape> shape;
	if (body.Shape == ShapeType::Circle)
	{
		shape = new JPH::SphereShape(ToMeters(body.HalfExtentsPx.X));
	}
	else
	{
		float hx = ToMeters(body.HalfExtentsPx.X);
		float hy = ToMeters(body.HalfExtentsPx.Y);
		float convexRadius = std::min(JPH::cDefaultConvexRadius, 0.5f * std::min(hx, hy));
		shape = new JPH::BoxShape(JPH::Vec3(hx, hy, 0.5f), convexRadius);
	}

	JPH::EMotionType motion = JPH::EMotionType::Static;
	switch (body.Type)
	{
	case BodyType::Kinematic: motion = JPH::EMotionType::Kinematic; break;
	case BodyType::Dynamic:   motion = JPH::EMotionType::Dynamic; break;
	default: break;
	}

	bool isStatic = body.Type == BodyType::Static;
	JPH::ObjectLayer objectLayer = mJolt.InternLayer(body.Layer, body.CollidesWith, isStatic);
	if (objectLayer == JPH::cObjectLayerInvalid)
	{
		printf("PhysicsWorld: body not created\n");
		return;
	}

	JPH::BodyCreationSettings settings(shape, CenterFromTransform(transform, body), RotationFromTransform(transform),
		motion, objectLayer);
	settings.mAllowedDOFs = body.LockRotation
		? (JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY)
		: JPH::EAllowedDOFs::Plane2D;
	settings.mMotionQuality = body.UseCCD ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete;
	settings.mIsSensor = body.IsSensor;
	settings.mAllowSleeping = body.AllowSleeping;
	settings.mRestitution = body.Restitution;
	settings.mFriction = body.Friction;
	settings.mLinearDamping = body.LinearDamping;
	settings.mAngularDamping = body.AngularDamping;
	settings.mGravityFactor = body.GravityFactor;

	body.Id = mJolt.AddBody(entity, settings, isStatic);
}

void PhysicsWorld::DestroyBody(RigidBody& body)
{
	mJolt.RemoveBody(body.Id);
}

int PhysicsWorld::Step(double deltaTime, const std::function<void(float)>& kinematicSync)
{
	return mJolt.Step(deltaTime, kinematicSync);
}

void PhysicsWorld::PushKinematic(Transform& transform, const RigidBody& body, float stepTime)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid() || stepTime <= 0.0f) return;
	mJolt.GetBodyInterface().MoveKinematic(body.Id, CenterFromTransform(transform, body), RotationFromTransform(transform), stepTime);
}

void PhysicsWorld::PullTransform(Transform& transform, const RigidBody& body)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;

	const JPH::BodyInterface& bodies = mJolt.GetBodyInterface();
	transform.Position = ToPixels(bodies.GetPosition(body.Id)) - body.HalfExtentsPx;
	float radians = bodies.GetRotation(body.Id).GetRotationAngle(JPH::Vec3::sAxisZ());
	transform.setRotation(radians * kRadToDeg);
}

void PhysicsWorld::SetLinearVelocity(const RigidBody& body, Vector2 pxPerSec)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().SetLinearVelocity(body.Id, JPH::Vec3(ToMeters(pxPerSec.X), ToMeters(pxPerSec.Y), 0.0f));
}

Vector2 PhysicsWorld::GetLinearVelocity(const RigidBody& body) const
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return Vector2();
	JPH::Vec3 v = mJolt.GetBodyInterface().GetLinearVelocity(body.Id);
	return Vector2(ToPixels(v.GetX()), ToPixels(v.GetY()));
}

void PhysicsWorld::SetAngularVelocity(const RigidBody& body, float degPerSec)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().SetAngularVelocity(body.Id, JPH::Vec3(0.0f, 0.0f, degPerSec * kDegToRad));
}

float PhysicsWorld::GetAngularVelocity(const RigidBody& body) const
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return 0.0f;
	return mJolt.GetBodyInterface().GetAngularVelocity(body.Id).GetZ() * kRadToDeg;
}

void PhysicsWorld::DrainContacts(ContactMap& out)
{
	out.clear();
	mJolt.DrainContacts(mResolved);

	for (const JoltWorld::ResolvedContact& c : mResolved)
	{
		Vector2 point(ToPixels(c.Point.GetX()), ToPixels(c.Point.GetY()));
		Vector2 normal(c.Normal.GetX(), c.Normal.GetY());
		out[c.A].push_back({ c.B, point, normal, c.Phase });
		out[c.B].push_back({ c.A, point, normal * -1.0f, c.Phase });
	}
}
