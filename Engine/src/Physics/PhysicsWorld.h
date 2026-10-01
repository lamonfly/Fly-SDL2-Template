#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "PhysicsConfig.h"
#include "Layers.h"
#include "RigidBody.h"
#include "Contact.h"
#include "Transform.h"

// One Jolt PhysicsSystem per Scene
class PhysicsWorld
{
public:
	using ContactMap = std::unordered_map<entt::entity, std::vector<Contact>>;

	PhysicsWorld() = default;
	~PhysicsWorld();

	PhysicsWorld(const PhysicsWorld&) = delete;
	PhysicsWorld& operator=(const PhysicsWorld&) = delete;

	void Init(const PhysicsConfig& config);
	bool IsInitialized() const { return mSystem != nullptr; }
	bool HasBodies() const { return !mEntityByBody.empty(); }
	const PhysicsConfig& GetConfig() const { return mConfig; }

	void CreateBody(entt::entity entity, Transform& transform, RigidBody& body);
	void DestroyBody(RigidBody& body);

	// Returns fixed steps run
	int Step(double deltaTime, const std::function<void(float totalStepTime)>& kinematicSync);

	void PushKinematic(Transform& transform, const RigidBody& body, float stepTime);
	void PullTransform(Transform& transform, const RigidBody& body);

	// px/s and deg/s
	void SetLinearVelocity(const RigidBody& body, Vector2 pxPerSec);
	Vector2 GetLinearVelocity(const RigidBody& body) const;
	void SetAngularVelocity(const RigidBody& body, float degPerSec);
	float GetAngularVelocity(const RigidBody& body) const;

	// Contacts from last Step
	void DrainContacts(ContactMap& out);

	float ToMeters(float px) const { return px / mConfig.PixelsPerMeter; }
	float ToPixels(float m) const { return m * mConfig.PixelsPerMeter; }
	JPH::RVec3 ToMeters(Vector2 px) const { return JPH::RVec3(ToMeters(px.X), ToMeters(px.Y), 0.0f); }
	Vector2 ToPixels(JPH::RVec3Arg m) const { return Vector2(ToPixels(static_cast<float>(m.GetX())), ToPixels(static_cast<float>(m.GetY()))); }

	// Raw Jolt access
	JPH::PhysicsSystem& GetSystem() { return *mSystem; }
	JPH::BodyInterface& GetBodyInterface() { return mSystem->GetBodyInterface(); }

private:
	struct RawContact
	{
		JPH::BodyID A;
		JPH::BodyID B;
		Vector2 Point;   // meters
		Vector2 Normal;  // A to B
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

	JPH::RVec3 CenterFromTransform(Transform& transform, const RigidBody& body) const;
	JPH::Quat RotationFromTransform(Transform& transform) const;
	entt::entity EntityFor(const JPH::BodyID& id) const;

	PhysicsConfig mConfig;
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
