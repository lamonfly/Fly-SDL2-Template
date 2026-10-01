#include "JoltWorld.h"
#include "JoltGlobals.h"

#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/SubShapeIDPair.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <cstdio>

JoltWorld::~JoltWorld()
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

void JoltWorld::Init(const Capacities& capacities, const Stepping& stepping, JPH::Vec3Arg gravity)
{
	mStepping = stepping;

	mBroadPhaseInterface = std::make_unique<LayerBroadPhaseInterface>(mLayers);
	mObjectVsBroadPhaseFilter = std::make_unique<LayerObjectVsBroadPhaseFilter>(mLayers);
	mObjectPairFilter = std::make_unique<LayerPairFilter>(mLayers);

	mSystem = std::make_unique<JPH::PhysicsSystem>();
	mSystem->Init(capacities.MaxBodies, 0, capacities.MaxBodyPairs, capacities.MaxContactConstraints,
		*mBroadPhaseInterface, *mObjectVsBroadPhaseFilter, *mObjectPairFilter);
	mSystem->SetGravity(gravity);
	mSystem->SetContactListener(&mListener);

	mAccumulator = 0.0;
}

JPH::ObjectLayer JoltWorld::InternLayer(CollisionMask group, CollisionMask mask, bool isStatic)
{
	if (mLayers.Size() >= LayerTable::MaxEntries)
	{
		printf("JoltWorld: too many distinct layer/mask combinations (%zu)\n", mLayers.Size());
		return JPH::cObjectLayerInvalid;
	}
	return mLayers.Intern(group, mask, isStatic);
}

entt::entity JoltWorld::EntityFor(const JPH::BodyID& id) const
{
	auto it = mEntityByBody.find(id.GetIndexAndSequenceNumber());
	return it != mEntityByBody.end() ? it->second : entt::null;
}

JPH::BodyID JoltWorld::AddBody(entt::entity entity, JPH::BodyCreationSettings& settings, bool isStatic)
{
	settings.mUserData = static_cast<JPH::uint64>(entt::to_integral(entity));

	JPH::BodyID id = mSystem->GetBodyInterface().CreateAndAddBody(settings, isStatic ? JPH::EActivation::DontActivate : JPH::EActivation::Activate);
	if (id.IsInvalid())
	{
		printf("JoltWorld: failed to create body (MaxBodies reached?)\n");
		return id;
	}

	mEntityByBody[id.GetIndexAndSequenceNumber()] = entity;
	if (isStatic) mBroadPhaseDirty = true;
	return id;
}

void JoltWorld::RemoveBody(JPH::BodyID& id)
{
	if (!mSystem || id.IsInvalid()) return;

	JPH::BodyInterface& bodies = mSystem->GetBodyInterface();
	bodies.RemoveBody(id);
	bodies.DestroyBody(id);
	mEntityByBody.erase(id.GetIndexAndSequenceNumber());
	id = JPH::BodyID();
}

int JoltWorld::Step(double deltaTime, const std::function<void(float)>& kinematicSync)
{
	{
		std::lock_guard<std::mutex> lock(mListener.Mutex);
		mListener.Events.clear();
	}

	if (!mSystem) return 0;

	mAccumulator += deltaTime;
	int steps = static_cast<int>(mAccumulator / mStepping.FixedTimeStep);
	if (steps <= 0) return 0;

	if (steps > mStepping.MaxSubSteps)
	{
		steps = mStepping.MaxSubSteps;
		mAccumulator = 0.0; // drop excess
	}
	else
	{
		mAccumulator -= steps * static_cast<double>(mStepping.FixedTimeStep);
	}

	if (mBroadPhaseDirty)
	{
		mSystem->OptimizeBroadPhase();
		mBroadPhaseDirty = false;
	}

	float total = steps * mStepping.FixedTimeStep;
	if (kinematicSync) kinematicSync(total);

	for (int i = 0; i < steps; i++)
	{
		JPH::EPhysicsUpdateError err = mSystem->Update(mStepping.FixedTimeStep, 1, JoltGlobals::GetTempAllocator(), JoltGlobals::GetJobSystem());
		if (err != JPH::EPhysicsUpdateError::None)
			printf("JoltWorld: update error 0x%x (raise MaxBodyPairs or MaxContactConstraints)\n", static_cast<unsigned>(err));
	}

	return steps;
}

void JoltWorld::DrainContacts(std::vector<ResolvedContact>& out)
{
	out.clear();

	std::lock_guard<std::mutex> lock(mListener.Mutex);
	for (const RawContact& raw : mListener.Events)
	{
		entt::entity a = EntityFor(raw.A);
		entt::entity b = EntityFor(raw.B);
		if (a == entt::null || b == entt::null) continue; // body destroyed

		out.push_back({ a, b, raw.Point, raw.Normal, raw.Phase });
	}
}

// Jolt worker threads, read-only

void JoltWorld::Listener::Push(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, ContactPhase phase)
{
	RawContact c;
	c.A = a.GetID();
	c.B = b.GetID();
	c.Phase = phase;
	c.Normal = m.mWorldSpaceNormal;

	JPH::uint count = m.mRelativeContactPointsOn1.size();
	if (count > 0)
	{
		JPH::RVec3 sum = JPH::RVec3::sZero();
		for (JPH::uint i = 0; i < count; i++)
			sum += m.GetWorldSpaceContactPointOn1(i);
		sum /= static_cast<float>(count);
		c.Point = JPH::Vec3(static_cast<float>(sum.GetX()), static_cast<float>(sum.GetY()), static_cast<float>(sum.GetZ()));
	}

	std::lock_guard<std::mutex> lock(Mutex);
	Events.push_back(c);
}

void JoltWorld::Listener::OnContactAdded(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings&)
{
	Push(a, b, m, ContactPhase::Enter);
}

void JoltWorld::Listener::OnContactPersisted(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings&)
{
	Push(a, b, m, ContactPhase::Stay);
}

void JoltWorld::Listener::OnContactRemoved(const JPH::SubShapeIDPair& pair)
{
	RawContact c;
	c.A = pair.GetBody1ID();
	c.B = pair.GetBody2ID();
	c.Phase = ContactPhase::Exit;

	std::lock_guard<std::mutex> lock(Mutex);
	Events.push_back(c);
}
