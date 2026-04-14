#pragma once
#include "Shapes/Shape.h"
#include "CollisionLayer.h"

struct Collider
{
private:
	Shape* mShape;

	std::unordered_map<uint32_t, Collision*> mPreviousCollisions = {};
	std::unordered_map<uint32_t, Collision*> mCollisions = {};

	AABB mCachedAABB;
	bool mAABBDirty = true;

public:
	bool IsStatic = false;
	CollisionLayer Layer = CollisionLayer::Default;
	uint16_t CollidesWith = static_cast<uint16_t>(CollisionLayer::All);

	bool UseCCD = false;
	float CCDSpeedThreshold = 100.0f;

	Collider(Shape* shape) : mShape(shape) {};

	Shape* GetShape() { return mShape; }

	inline void Clear() {
		mPreviousCollisions.clear();
		mPreviousCollisions = mCollisions;
		mCollisions.clear();
	};

	inline void AddCollision(uint32_t id, Collision* collision) { mCollisions[id] = collision; };

	inline bool HasEntered(uint32_t id) { return mPreviousCollisions.find(id) == mPreviousCollisions.end(); };

	inline std::unordered_map<uint32_t, Collision*> GetCollisions() { return mCollisions; };

	inline Collision* DoesCollide(Transform& current, Transform& other, Collider& collider) {
		return mShape->Collide(current, other, collider.GetShape());
	}

	inline const AABB& GetAABB(Transform& transform) {
		if (mAABBDirty || !IsStatic) {
			mCachedAABB = mShape->GetAABB(transform);
			mAABBDirty = false;
		}
		return mCachedAABB;
	}

	inline BoundingSphere GetBoundingSphere(Transform& transform) const {
		return mShape->GetBoundingSphere(transform);
	}

	inline void MarkDirty() { 
		mAABBDirty = true; 
	}

	inline bool CanCollideWith(const Collider& other) const {
		return (static_cast<uint16_t>(Layer) & other.CollidesWith) != 0 &&
			   (static_cast<uint16_t>(other.Layer) & CollidesWith) != 0;
	}
};

