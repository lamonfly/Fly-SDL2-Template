#include "PhysicsWorld3D.h"
#include "JoltMath.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <algorithm>
#include <cstdio>

using namespace JoltMath;

void PhysicsWorld3D::Init(const PhysicsConfig3D& config)
{
	mConfig = config;

	JoltWorld::Capacities capacities;
	capacities.MaxBodies = config.MaxBodies;
	capacities.MaxBodyPairs = config.MaxBodyPairs;
	capacities.MaxContactConstraints = config.MaxContactConstraints;

	JoltWorld::Stepping stepping;
	stepping.FixedTimeStep = config.FixedTimeStep;
	stepping.MaxSubSteps = config.MaxSubSteps;

	mJolt.Init(capacities, stepping, ToJolt(config.Gravity));
}

JPH::RefConst<JPH::Shape> PhysicsWorld3D::MakeShape(const RigidBody3D& body) const
{
	constexpr float kMin = 0.001f;
	switch (body.Shape)
	{
	case ShapeType3D::Sphere:
		return new JPH::SphereShape(std::max(body.Radius, kMin));

	case ShapeType3D::Capsule:
		return new JPH::CapsuleShape(std::max(body.HalfHeight, kMin), std::max(body.Radius, kMin));

	case ShapeType3D::Cylinder:
	{
		float h = std::max(body.HalfHeight, kMin);
		float r = std::max(body.Radius, kMin);
		float convexRadius = std::min(JPH::cDefaultConvexRadius, 0.5f * std::min(h, r));
		return new JPH::CylinderShape(h, r, convexRadius);
	}

	default:
	{
		glm::vec3 he = glm::max(body.HalfExtents, glm::vec3(kMin));
		float convexRadius = std::min(JPH::cDefaultConvexRadius, 0.5f * std::min({ he.x, he.y, he.z }));
		return new JPH::BoxShape(ToJolt(he), convexRadius);
	}
	}
}

void PhysicsWorld3D::CreateBody(entt::entity entity, const Transform3& transform, RigidBody3D& body)
{
	if (!mJolt.IsInitialized()) Init(mConfig);

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
		printf("PhysicsWorld3D: body not created\n");
		return;
	}

	JPH::BodyCreationSettings settings(MakeShape(body), ToJoltR(transform.Position), ToJolt(transform.Rotation), motion, objectLayer);
	settings.mAllowedDOFs = body.LockRotation
		? (JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ)
		: JPH::EAllowedDOFs::All;
	settings.mMotionQuality = body.UseCCD ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete;
	settings.mIsSensor = body.IsSensor;
	settings.mAllowSleeping = body.AllowSleeping;
	settings.mRestitution = body.Restitution;
	settings.mFriction = body.Friction;
	settings.mLinearDamping = body.LinearDamping;
	settings.mAngularDamping = body.AngularDamping;
	settings.mGravityFactor = body.GravityFactor;
	if (body.Mass > 0.0f)
	{
		settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
		settings.mMassPropertiesOverride.mMass = body.Mass;
	}

	body.Id = mJolt.AddBody(entity, settings, isStatic);
}

void PhysicsWorld3D::DestroyBody(RigidBody3D& body)
{
	mJolt.RemoveBody(body.Id);
}

int PhysicsWorld3D::Step(double deltaTime, const std::function<void(float)>& kinematicSync)
{
	return mJolt.Step(deltaTime, kinematicSync);
}

void PhysicsWorld3D::PushKinematic(const Transform3& transform, const RigidBody3D& body, float stepTime)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid() || stepTime <= 0.0f) return;
	mJolt.GetBodyInterface().MoveKinematic(body.Id, ToJoltR(transform.Position), ToJolt(transform.Rotation), stepTime);
}

void PhysicsWorld3D::PullTransform(Transform3& transform, const RigidBody3D& body)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;

	JPH::RVec3 position;
	JPH::Quat rotation;
	mJolt.GetBodyInterface().GetPositionAndRotation(body.Id, position, rotation);
	transform.Position = ToGlmR(position);
	transform.Rotation = ToGlm(rotation);
}

void PhysicsWorld3D::SetLinearVelocity(const RigidBody3D& body, const glm::vec3& v)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().SetLinearVelocity(body.Id, ToJolt(v));
}

glm::vec3 PhysicsWorld3D::GetLinearVelocity(const RigidBody3D& body) const
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return glm::vec3(0.0f);
	return ToGlm(mJolt.GetBodyInterface().GetLinearVelocity(body.Id));
}

void PhysicsWorld3D::SetAngularVelocity(const RigidBody3D& body, const glm::vec3& v)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().SetAngularVelocity(body.Id, ToJolt(v));
}

glm::vec3 PhysicsWorld3D::GetAngularVelocity(const RigidBody3D& body) const
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return glm::vec3(0.0f);
	return ToGlm(mJolt.GetBodyInterface().GetAngularVelocity(body.Id));
}

void PhysicsWorld3D::AddForce(const RigidBody3D& body, const glm::vec3& newtons)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().AddForce(body.Id, ToJolt(newtons));
}

void PhysicsWorld3D::AddImpulse(const RigidBody3D& body, const glm::vec3& newtonSeconds)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().AddImpulse(body.Id, ToJolt(newtonSeconds));
}

void PhysicsWorld3D::AddTorque(const RigidBody3D& body, const glm::vec3& newtonMeters)
{
	if (!mJolt.IsInitialized() || body.Id.IsInvalid()) return;
	mJolt.GetBodyInterface().AddTorque(body.Id, ToJolt(newtonMeters));
}

void PhysicsWorld3D::DrainContacts(ContactMap& out)
{
	out.clear();
	mJolt.DrainContacts(mResolved);

	for (const JoltWorld::ResolvedContact& c : mResolved)
	{
		glm::vec3 point = ToGlm(c.Point);
		glm::vec3 normal = ToGlm(c.Normal);
		out[c.A].push_back({ c.B, point, normal, c.Phase });
		out[c.B].push_back({ c.A, point, -normal, c.Phase });
	}
}
