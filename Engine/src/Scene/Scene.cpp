#include "Scene.h"
#include "../Physics/Collider/Collider.h"

Scene::~Scene() {
	mRegistry.clear();
}

void Scene::UpdatePhysics(double deltaTime)
{
	std::vector<entt::entity> seen;

	for (auto const &[entity, transform, collider] : mRegistry.view<Transform, Collider>().each()) {
		// Clear all previous update collision
		collider.Clear();

		// Get current update collision
		for (auto other : seen) {
			const auto [otherTransform, otherCollider] = mRegistry.get<Transform, Collider>(other);
			auto result = collider.DoesCollide(transform, otherTransform, otherCollider);

			if (result != NULL) {
				uint32_t entityId = entt::to_integral(entity);
				uint32_t otherId = entt::to_integral(other);
				collider.AddCollision(otherId, result);
				otherCollider.AddCollision(entityId, result);
			}
		}

		seen.push_back(entity);
	}

	seen.clear();
}
