#include "Scene.h"
#include <cstdio>

Scene::Scene()
{
	mRegistry.on_construct<RigidBody>().connect<&Scene::OnRigidBodyConstruct>(*this);
	mRegistry.on_destroy<RigidBody>().connect<&Scene::OnRigidBodyDestroy>(*this);
}

Scene::~Scene() {
	mRegistry.clear(); // destroys bodies first
}

void Scene::OnRigidBodyConstruct(entt::registry& registry, entt::entity entity)
{
	Transform* transform = registry.try_get<Transform>(entity);
	if (transform == nullptr)
	{
		printf("Scene: RigidBody emplaced on entity %u without a Transform; body not created\n", entt::to_integral(entity));
		return;
	}
	mPhysics.CreateBody(entity, *transform, registry.get<RigidBody>(entity));
}

void Scene::OnRigidBodyDestroy(entt::registry& registry, entt::entity entity)
{
	mPhysics.DestroyBody(registry.get<RigidBody>(entity));
}

void Scene::UpdatePhysics(double deltaTime)
{
	auto view = mRegistry.view<Transform, RigidBody>();

	int steps = mPhysics.Step(deltaTime, [&](float totalStepTime) {
		for (auto&& [entity, transform, body] : view.each()) {
			if (body.Type == BodyType::Kinematic)
				mPhysics.PushKinematic(transform, body, totalStepTime);
		}
	});

	if (steps == 0) {
		mContacts.clear();
		return;
	}

	for (auto&& [entity, transform, body] : view.each()) {
		if (body.Type != BodyType::Static)
			mPhysics.PullTransform(transform, body);
	}

	mPhysics.DrainContacts(mContacts);
}

void Scene::SetPhysicsConfig(const PhysicsConfig& config)
{
	if (mPhysics.HasBodies())
	{
		printf("Scene: SetPhysicsConfig ignored, bodies already exist\n");
		return;
	}
	mPhysics.Init(config);
}

const PhysicsConfig& Scene::GetPhysicsConfig() const
{
	return mPhysics.GetConfig();
}

const std::vector<Contact>& Scene::GetContacts(entt::entity entity) const
{
	static const std::vector<Contact> empty;
	auto it = mContacts.find(entity);
	return it != mContacts.end() ? it->second : empty;
}
