#include "PhysicsWorld.h"
#include "JoltGlobals.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/SubShapeIDPair.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

static constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
static constexpr float kRadToDeg = 180.0f / 3.14159265358979323846f;

PhysicsWorld::~PhysicsWorld()
{
	if (mSystem)
	{
		// Leftover bodies
		JPH::BodyInterface& bodies = mSystem->GetBodyInterface();
		for (auto& [key, entity] : mEntityByBody)
		{
			JPH::BodyID id(key);
			bodies.RemoveBody(id);
			bodies.DestroyBody(id);
		}
		mEntityByBody.clear();
		mSystem->SetContactListener(nullptr);
	}
}

void PhysicsWorld::Init(const PhysicsConfig& config)
{
	mConfig = config;

	mBroadPhaseInterface = std::make_unique<LayerBroadPhaseInterface>(mLayers);
	mObjectVsBroadPhaseFilter = std::make_unique<LayerObjectVsBroadPhaseFilter>(mLayers);
	mObjectPairFilter = std::make_unique<LayerPairFilter>(mLayers);

	mSystem = std::make_unique<JPH::PhysicsSystem>();
	mSystem->Init(config.MaxBodies, 0, config.MaxBodyPairs, config.MaxContactConstraints,
		*mBroadPhaseInterface, *mObjectVsBroadPhaseFilter, *mObjectPairFilter);
	mSystem->SetGravity(JPH::Vec3(config.Gravity.X, config.Gravity.Y, 0.0f));
	mSystem->SetContactListener(&mListener);

	mAccumulator = 0.0;
}

JPH::RVec3 PhysicsWorld::CenterFromTransform(Transform& transform, const RigidBody& body) const
{
	return ToMeters(transform.Position + body.HalfExtentsPx);
}

JPH::Quat PhysicsWorld::RotationFromTransform(Transform& transform) const
{
	return JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), static_cast<float>(transform.getRotation()) * kDegToRad);
}

entt::entity PhysicsWorld::EntityFor(const JPH::BodyID& id) const
{
	auto it = mEntityByBody.find(id.GetIndexAndSequenceNumber());
	return it != mEntityByBody.end() ? it->second : entt::null;
}

void PhysicsWorld::CreateBody(entt::entity entity, Transform& transform, RigidBody& body)
{
	if (!mSystem) Init(mConfig);

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

	if (mLayers.Size() >= LayerTable::MaxEntries)
	{
		printf("PhysicsWorld: too many distinct layer/mask combinations (%zu); body not created\n", mLayers.Size());
		return;
	}
	JPH::ObjectLayer objectLayer = mLayers.Intern(body.Layer, body.CollidesWith, body.Type == BodyType::Static);

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
	settings.mUserData = static_cast<JPH::uint64>(entt::to_integral(entity));

	body.Id = mSystem->GetBodyInterface().CreateAndAddBody(settings, JPH::EActivation::Activate);
	if (body.Id.IsInvalid())
	{
		printf("PhysicsWorld: failed to create body (MaxBodies=%u reached?)\n", mConfig.MaxBodies);
		return;
	}

	mEntityByBody[body.Id.GetIndexAndSequenceNumber()] = entity;
	if (body.Type == BodyType::Static) mBroadPhaseDirty = true;
}

void PhysicsWorld::DestroyBody(RigidBody& body)
{
	if (!mSystem || body.Id.IsInvalid()) return;

	JPH::BodyInterface& bodies = mSystem->GetBodyInterface();
	bodies.RemoveBody(body.Id);
	bodies.DestroyBody(body.Id);
	mEntityByBody.erase(body.Id.GetIndexAndSequenceNumber());
	body.Id = JPH::BodyID();
}

int PhysicsWorld::Step(double deltaTime, const std::function<void(float)>& kinematicSync)
{
	{
		std::lock_guard<std::mutex> lock(mListener.Mutex);
		mListener.Events.clear();
	}

	if (!mSystem) return 0;

	mAccumulator += deltaTime;
	int steps = static_cast<int>(mAccumulator / mConfig.FixedTimeStep);
	if (steps <= 0) return 0;

	if (steps > mConfig.MaxSubSteps)
	{
		steps = mConfig.MaxSubSteps;
		mAccumulator = 0.0; // drop excess
	}
	else
	{
		mAccumulator -= steps * static_cast<double>(mConfig.FixedTimeStep);
	}

	if (mBroadPhaseDirty)
	{
		mSystem->OptimizeBroadPhase();
		mBroadPhaseDirty = false;
	}

	float total = steps * mConfig.FixedTimeStep;
	if (kinematicSync) kinematicSync(total);

	for (int i = 0; i < steps; i++)
	{
		mSystem->Update(mConfig.FixedTimeStep, 1, JoltGlobals::GetTempAllocator(), JoltGlobals::GetJobSystem());
	}

	return steps;
}

void PhysicsWorld::PushKinematic(Transform& transform, const RigidBody& body, float stepTime)
{
	if (!mSystem || body.Id.IsInvalid() || stepTime <= 0.0f) return;
	mSystem->GetBodyInterface().MoveKinematic(body.Id, CenterFromTransform(transform, body), RotationFromTransform(transform), stepTime);
}

void PhysicsWorld::PullTransform(Transform& transform, const RigidBody& body)
{
	if (!mSystem || body.Id.IsInvalid()) return;

	const JPH::BodyInterface& bodies = mSystem->GetBodyInterface();
	transform.Position = ToPixels(bodies.GetPosition(body.Id)) - body.HalfExtentsPx;
	float radians = bodies.GetRotation(body.Id).GetRotationAngle(JPH::Vec3::sAxisZ());
	transform.setRotation(radians * kRadToDeg);
}

void PhysicsWorld::SetLinearVelocity(const RigidBody& body, Vector2 pxPerSec)
{
	if (!mSystem || body.Id.IsInvalid()) return;
	mSystem->GetBodyInterface().SetLinearVelocity(body.Id, JPH::Vec3(ToMeters(pxPerSec.X), ToMeters(pxPerSec.Y), 0.0f));
}

Vector2 PhysicsWorld::GetLinearVelocity(const RigidBody& body) const
{
	if (!mSystem || body.Id.IsInvalid()) return Vector2();
	JPH::Vec3 v = mSystem->GetBodyInterface().GetLinearVelocity(body.Id);
	return Vector2(ToPixels(v.GetX()), ToPixels(v.GetY()));
}

void PhysicsWorld::SetAngularVelocity(const RigidBody& body, float degPerSec)
{
	if (!mSystem || body.Id.IsInvalid()) return;
	mSystem->GetBodyInterface().SetAngularVelocity(body.Id, JPH::Vec3(0.0f, 0.0f, degPerSec * kDegToRad));
}

float PhysicsWorld::GetAngularVelocity(const RigidBody& body) const
{
	if (!mSystem || body.Id.IsInvalid()) return 0.0f;
	return mSystem->GetBodyInterface().GetAngularVelocity(body.Id).GetZ() * kRadToDeg;
}

void PhysicsWorld::DrainContacts(ContactMap& out)
{
	out.clear();

	std::lock_guard<std::mutex> lock(mListener.Mutex);
	for (const RawContact& raw : mListener.Events)
	{
		entt::entity a = EntityFor(raw.A);
		entt::entity b = EntityFor(raw.B);
		if (a == entt::null || b == entt::null) continue; // body destroyed

		Vector2 point(ToPixels(raw.Point.X), ToPixels(raw.Point.Y));
		out[a].push_back({ b, point, raw.Normal, raw.Phase });
		out[b].push_back({ a, point, raw.Normal * -1.0f, raw.Phase });
	}
}

// Jolt worker threads, read-only

void PhysicsWorld::Listener::Push(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, ContactPhase phase)
{
	RawContact c;
	c.A = a.GetID();
	c.B = b.GetID();
	c.Phase = phase;
	c.Normal = Vector2(m.mWorldSpaceNormal.GetX(), m.mWorldSpaceNormal.GetY());

	JPH::uint count = m.mRelativeContactPointsOn1.size();
	if (count > 0)
	{
		JPH::RVec3 sum = JPH::RVec3::sZero();
		for (JPH::uint i = 0; i < count; i++)
			sum += m.GetWorldSpaceContactPointOn1(i);
		sum /= static_cast<float>(count);
		c.Point = Vector2(static_cast<float>(sum.GetX()), static_cast<float>(sum.GetY()));
	}

	std::lock_guard<std::mutex> lock(Mutex);
	Events.push_back(c);
}

void PhysicsWorld::Listener::OnContactAdded(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings&)
{
	Push(a, b, m, ContactPhase::Enter);
}

void PhysicsWorld::Listener::OnContactPersisted(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings&)
{
	Push(a, b, m, ContactPhase::Stay);
}

void PhysicsWorld::Listener::OnContactRemoved(const JPH::SubShapeIDPair& pair)
{
	RawContact c;
	c.A = pair.GetBody1ID();
	c.B = pair.GetBody2ID();
	c.Phase = ContactPhase::Exit;

	std::lock_guard<std::mutex> lock(Mutex);
	Events.push_back(c);
}
