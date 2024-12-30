#pragma once
#include "Shapes/Shape.h"

struct Collider
{
private:
	// hold info about shape, call right function
	Shape* mShape;

	std::unordered_map<uint32_t, Collision*> mPreviousCollisions = {};
	std::unordered_map<uint32_t, Collision*> mCollisions = {};
public:
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

	//Check if colliders intersect
	inline Collision* DoesCollide(Transform& current, Transform& other, Collider& collider) {
		return mShape->Collide(current, other, collider.GetShape());
	}
};

