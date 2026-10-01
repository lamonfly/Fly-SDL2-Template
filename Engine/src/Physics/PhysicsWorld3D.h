#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

#include "JoltWorld.h"
#include "PhysicsConfig3D.h"
#include "RigidBody3D.h"
#include "Contact3D.h"
#include "../Math/Transform3.h"

// 3D world on Jolt, meters and radians
class PhysicsWorld3D
{
public:
	using ContactMap = std::unordered_map<entt::entity, std::vector<Contact3D>>;

	PhysicsWorld3D() = default;
	~PhysicsWorld3D() = default;

	PhysicsWorld3D(const PhysicsWorld3D&) = delete;
	PhysicsWorld3D& operator=(const PhysicsWorld3D&) = delete;

	void Init(const PhysicsConfig3D& config);
	bool IsInitialized() const { return mJolt.IsInitialized(); }
	bool HasBodies() const { return mJolt.HasBodies(); }
	const PhysicsConfig3D& GetConfig() const { return mConfig; }

	void CreateBody(entt::entity entity, const Transform3& transform, RigidBody3D& body);
	void DestroyBody(RigidBody3D& body);

	// Returns fixed steps run
	int Step(double deltaTime, const std::function<void(float totalStepTime)>& kinematicSync);

	void PushKinematic(const Transform3& transform, const RigidBody3D& body, float stepTime);
	void PullTransform(Transform3& transform, const RigidBody3D& body);

	// m/s and rad/s
	void SetLinearVelocity(const RigidBody3D& body, const glm::vec3& v);
	glm::vec3 GetLinearVelocity(const RigidBody3D& body) const;
	void SetAngularVelocity(const RigidBody3D& body, const glm::vec3& v);
	glm::vec3 GetAngularVelocity(const RigidBody3D& body) const;

	// Applied on next Step
	void AddForce(const RigidBody3D& body, const glm::vec3& newtons);
	void AddImpulse(const RigidBody3D& body, const glm::vec3& newtonSeconds);
	void AddTorque(const RigidBody3D& body, const glm::vec3& newtonMeters);

	// Contacts from last Step
	void DrainContacts(ContactMap& out);

	// Raw Jolt access
	JPH::PhysicsSystem& GetSystem() { return mJolt.GetSystem(); }
	JPH::BodyInterface& GetBodyInterface() { return mJolt.GetBodyInterface(); }

private:
	JPH::RefConst<JPH::Shape> MakeShape(const RigidBody3D& body) const;

	PhysicsConfig3D mConfig;
	JoltWorld mJolt;
	std::vector<JoltWorld::ResolvedContact> mResolved;
};
