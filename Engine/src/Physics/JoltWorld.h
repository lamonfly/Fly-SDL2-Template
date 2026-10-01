#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <entt/entt.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Layers.h"
#include "Contact.h"

// Jolt lifecycle shared by 2D and 3D worlds
class JoltWorld
{
public:
	struct Capacities
	{
		uint32_t MaxBodies = 1024;
		uint32_t MaxBodyPairs = 1024;
		uint32_t MaxContactConstraints = 1024;
	};

	struct Stepping
	{
		float FixedTimeStep = 1.0f / 60.0f;
		int MaxSubSteps = 4;
	};

	// Meters, A to B
	struct ResolvedContact
	{
		entt::entity A = entt::null;
		entt::entity B = entt::null;
		JPH::Vec3 Point = JPH::Vec3::sZero();
		JPH::Vec3 Normal = JPH::Vec3::sZero();
		ContactPhase Phase = ContactPhase::Enter;
	};

	JoltWorld() = default;
	~JoltWorld();

	JoltWorld(const JoltWorld&) = delete;
	JoltWorld& operator=(const JoltWorld&) = delete;

	void Init(const Capacities& capacities, const Stepping& stepping, JPH::Vec3Arg gravity);
	bool IsInitialized() const { return mSystem != nullptr; }
	bool HasBodies() const { return !mEntityByBody.empty(); }

	// cObjectLayerInvalid when table full
	JPH::ObjectLayer InternLayer(CollisionMask group, CollisionMask mask, bool isStatic);

	// Sets user data, registers entity. Invalid id on failure
	JPH::BodyID AddBody(entt::entity entity, JPH::BodyCreationSettings& settings, bool isStatic);
	void RemoveBody(JPH::BodyID& id);

	// Returns fixed steps run
	int Step(double deltaTime, const std::function<void(float totalStepTime)>& kinematicSync);

	// Contacts from last Step, destroyed bodies dropped
	void DrainContacts(std::vector<ResolvedContact>& out);

	entt::entity EntityFor(const JPH::BodyID& id) const;

	JPH::PhysicsSystem& GetSystem() { return *mSystem; }
	JPH::BodyInterface& GetBodyInterface() { return mSystem->GetBodyInterface(); }
	const JPH::BodyInterface& GetBodyInterface() const { return mSystem->GetBodyInterface(); }

private:
	struct RawContact
	{
		JPH::BodyID A;
		JPH::BodyID B;
		JPH::Vec3 Point = JPH::Vec3::sZero();
		JPH::Vec3 Normal = JPH::Vec3::sZero();
		ContactPhase Phase = ContactPhase::Enter;
	};

	class Listener : public JPH::ContactListener
	{
	public:
		std::mutex Mutex;
		std::vector<RawContact> Events;

		void OnContactAdded(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings& s) override;
		void OnContactPersisted(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, JPH::ContactSettings& s) override;
		void OnContactRemoved(const JPH::SubShapeIDPair& pair) override;

	private:
		void Push(const JPH::Body& a, const JPH::Body& b, const JPH::ContactManifold& m, ContactPhase phase);
	};

	Stepping mStepping;
	std::unique_ptr<JPH::PhysicsSystem> mSystem;
	LayerTable mLayers;
	std::unique_ptr<LayerBroadPhaseInterface> mBroadPhaseInterface;
	std::unique_ptr<LayerObjectVsBroadPhaseFilter> mObjectVsBroadPhaseFilter;
	std::unique_ptr<LayerPairFilter> mObjectPairFilter;
	Listener mListener;

	std::unordered_map<JPH::uint32, entt::entity> mEntityByBody;
	double mAccumulator = 0.0;
	bool mBroadPhaseDirty = false;
};
