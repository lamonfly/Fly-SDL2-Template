#pragma once
#include "Scene.h"
#include "../Math/Transform3.h"
#include "../Physics/PhysicsWorld3D.h"
#include "../Physics/PhysicsConfig3D.h"
#include "../Physics/RigidBody3D.h"
#include "../Physics/Contact3D.h"
#include "../Graphics3D/Camera3D.h"

// Scene with 3D physics and GL rendering
class Scene3D : public Scene
{
public:
	Scene3D();
	~Scene3D() override;

	void Render(SDL_Renderer* renderer) final {}
	void Render3D(Renderer3D& renderer) override = 0;
	void UpdatePhysics(double deltaTime) override;

	// Before first RigidBody3D
	void SetPhysicsConfig3D(const PhysicsConfig3D& config);
	const PhysicsConfig3D& GetPhysicsConfig3D() const;

	// Contacts from last UpdatePhysics
	const std::vector<Contact3D>& GetContacts3D(entt::entity entity) const;

	PhysicsWorld3D& GetPhysics3D() { return mPhysics3D; }

	Camera3D Camera;

protected:
	PhysicsWorld3D mPhysics3D;   // destroyed after mRegistry
	PhysicsWorld3D::ContactMap mContacts3D;

	// Draws every Transform3 + MeshRenderer
	void RenderMeshes(Renderer3D& renderer);

private:
	void OnRigidBody3DConstruct(entt::registry& registry, entt::entity entity);
	void OnRigidBody3DDestroy(entt::registry& registry, entt::entity entity);
};
