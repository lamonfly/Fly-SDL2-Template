#include "Scene3D.h"
#include "../Graphics3D/Renderer3D.h"
#include "../Graphics3D/MeshRenderer.h"
#include <cstdio>

Scene3D::Scene3D()
{
	mRegistry.on_construct<RigidBody3D>().connect<&Scene3D::OnRigidBody3DConstruct>(*this);
	mRegistry.on_destroy<RigidBody3D>().connect<&Scene3D::OnRigidBody3DDestroy>(*this);
}

Scene3D::~Scene3D()
{
	mRegistry.clear(); // bodies before mPhysics3D
}

void Scene3D::OnRigidBody3DConstruct(entt::registry& registry, entt::entity entity)
{
	Transform3* transform = registry.try_get<Transform3>(entity);
	if (transform == nullptr)
	{
		printf("Scene3D: RigidBody3D emplaced on entity %u without a Transform3; body not created\n", entt::to_integral(entity));
		return;
	}
	mPhysics3D.CreateBody(entity, *transform, registry.get<RigidBody3D>(entity));
}

void Scene3D::OnRigidBody3DDestroy(entt::registry& registry, entt::entity entity)
{
	mPhysics3D.DestroyBody(registry.get<RigidBody3D>(entity));
}

void Scene3D::UpdatePhysics(double deltaTime)
{
	auto view = mRegistry.view<Transform3, RigidBody3D>();

	int steps = mPhysics3D.Step(deltaTime, [&](float totalStepTime) {
		for (auto&& [entity, transform, body] : view.each()) {
			if (body.Type == BodyType::Kinematic)
				mPhysics3D.PushKinematic(transform, body, totalStepTime);
		}
	});

	if (steps == 0) {
		mContacts3D.clear();
		return;
	}

	for (auto&& [entity, transform, body] : view.each()) {
		if (body.Type != BodyType::Static)
			mPhysics3D.PullTransform(transform, body);
	}

	mPhysics3D.DrainContacts(mContacts3D);
}

void Scene3D::SetPhysicsConfig3D(const PhysicsConfig3D& config)
{
	if (mPhysics3D.HasBodies())
	{
		printf("Scene3D: SetPhysicsConfig3D ignored, bodies already exist\n");
		return;
	}
	mPhysics3D.Init(config);
}

const PhysicsConfig3D& Scene3D::GetPhysicsConfig3D() const
{
	return mPhysics3D.GetConfig();
}

const std::vector<Contact3D>& Scene3D::GetContacts3D(entt::entity entity) const
{
	static const std::vector<Contact3D> empty;
	auto it = mContacts3D.find(entity);
	return it != mContacts3D.end() ? it->second : empty;
}

void Scene3D::RenderMeshes(Renderer3D& renderer)
{
	renderer.BeginFrame(Camera);
	for (auto&& [entity, transform, mesh] : mRegistry.view<Transform3, MeshRenderer>().each())
	{
		if (mesh.Visible && mesh.Mesh && mesh.Material)
			renderer.Draw(*mesh.Mesh, *mesh.Material, transform.ToMatrix());
	}
	renderer.EndFrame();
}
