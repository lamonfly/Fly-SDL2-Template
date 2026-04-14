#include "Scene.h"
#include "../Physics/Collider/Collider.h"

Scene::~Scene() {
	mRegistry.clear();
}

void Scene::UpdatePhysics(double deltaTime)
{
	std::vector<uint32_t> ids;
	std::vector<Collider*> colliders;
	std::vector<Transform*> transforms;

	for (auto const &[entity, transform, collider] : mRegistry.view<Transform, Collider>().each()) {
		collider.Clear();

		ids.push_back(entt::to_integral(entity));
		colliders.push_back(&collider);
		transforms.push_back(&transform);
	}

	if (ids.empty()) return;

	mBroadPhase.Update(ids, colliders, transforms);

	auto pairs = mBroadPhase.GetCollisionPairs(ids, colliders, transforms);

	for (const auto& pair : pairs)
	{
		entt::entity entityA = entt::entity(pair.IdA);
		entt::entity entityB = entt::entity(pair.IdB);

		auto [transformA, colliderA] = mRegistry.get<Transform, Collider>(entityA);
		auto [transformB, colliderB] = mRegistry.get<Transform, Collider>(entityB);

		auto result = colliderA.DoesCollide(transformA, transformB, colliderB);

		if (result != NULL) {
			colliderA.AddCollision(pair.IdB, result);
			colliderB.AddCollision(pair.IdA, result);
		}
	}
}

void Scene::SetSpatialPartitionConfig(const SpatialPartitionConfig& config)
{
	mBroadPhase = BroadPhase(config);
}

const SpatialPartitionConfig& Scene::GetSpatialPartitionConfig() const
{
	return mBroadPhase.GetConfig();
}
