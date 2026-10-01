#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include "../Physics/Transform.h"
#include "../Physics/PhysicsWorld.h"
#include "../Physics/PhysicsConfig.h"
#include "../Physics/RigidBody.h"
#include "../Physics/Contact.h"

class Renderer3D;

// Interface for scene setup
class Scene {
public:
	Scene();
	virtual ~Scene();

	virtual void Init() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void HandleEvent(SDL_Event& e) = 0;
	virtual void Render3D(Renderer3D& renderer) {}
	virtual void UpdatePhysics(double deltaTime);

	// Before first RigidBody
	void SetPhysicsConfig(const PhysicsConfig& config);
	const PhysicsConfig& GetPhysicsConfig() const;

	// Contacts from last UpdatePhysics
	const std::vector<Contact>& GetContacts(entt::entity entity) const;

	PhysicsWorld& GetPhysics() { return mPhysics; }

protected:
	PhysicsWorld mPhysics;   // destroyed after mRegistry
	entt::registry mRegistry;
	PhysicsWorld::ContactMap mContacts;

	template<typename T> inline void RenderType(SDL_Renderer* renderer) {
		for (auto&& [entity, transform, type] : mRegistry.view<Transform, T>().each()) {
			type.Render(renderer, transform);
		}
	}

	template<typename T> inline void UpdateType(double deltaTime) {
		for (auto&& [entity, type] : mRegistry.view<T>().each()) {
			type.Update(deltaTime);
		}
	}

	template<typename T> inline void HandleEventType(SDL_Event& currentEvent) {
		for (auto&& [entity, type] : mRegistry.view<T>().each()) {
			type.HandleEvent(currentEvent);
		}
	}

private:
	void OnRigidBodyConstruct(entt::registry& registry, entt::entity entity);
	void OnRigidBodyDestroy(entt::registry& registry, entt::entity entity);
};
