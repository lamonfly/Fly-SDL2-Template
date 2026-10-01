#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <entt/entt.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

#include "JoltWorld.h"
#include "PhysicsConfig.h"
#include "RigidBody.h"
#include "Contact.h"
#include "Transform.h"

// 2D world on Jolt, pixels and degrees
class PhysicsWorld
{
public:
	using ContactMap = std::unordered_map<entt::entity, std::vector<Contact>>;

	PhysicsWorld() = default;
	~PhysicsWorld() = default;

	PhysicsWorld(const PhysicsWorld&) = delete;
	PhysicsWorld& operator=(const PhysicsWorld&) = delete;

	void Init(const PhysicsConfig& config);
	bool IsInitialized() const { return mJolt.IsInitialized(); }
	bool HasBodies() const { return mJolt.HasBodies(); }
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
	JPH::PhysicsSystem& GetSystem() { return mJolt.GetSystem(); }
	JPH::BodyInterface& GetBodyInterface() { return mJolt.GetBodyInterface(); }

private:
	JPH::RVec3 CenterFromTransform(Transform& transform, const RigidBody& body) const;
	JPH::Quat RotationFromTransform(Transform& transform) const;

	PhysicsConfig mConfig;
	JoltWorld mJolt;
	std::vector<JoltWorld::ResolvedContact> mResolved;
};
