#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "CollisionLayer.h"

// ObjectLayer = LayerTable index

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer Static(0);
	static constexpr JPH::BroadPhaseLayer Moving(1);
	static constexpr JPH::uint Count = 2;
}

class LayerTable
{
public:
	struct Entry
	{
		CollisionMask Group = 0;
		CollisionMask Mask = 0;
		bool IsStatic = false;
	};

	// Main thread, outside Step
	JPH::ObjectLayer Intern(CollisionMask group, CollisionMask mask, bool isStatic)
	{
		uint64_t key = (static_cast<uint64_t>(group) << 32) | mask;
		auto& lookup = isStatic ? mStaticLookup : mMovingLookup;
		auto it = lookup.find(key);
		if (it != lookup.end()) return it->second;

		JPH::ObjectLayer layer = static_cast<JPH::ObjectLayer>(mEntries.size());
		mEntries.push_back({ group, mask, isStatic });
		lookup[key] = layer;
		return layer;
	}

	const Entry& Get(JPH::ObjectLayer layer) const { return mEntries[layer]; }
	size_t Size() const { return mEntries.size(); }

	static constexpr size_t MaxEntries = 1u << (sizeof(JPH::ObjectLayer) * 8);

private:
	std::vector<Entry> mEntries;
	std::unordered_map<uint64_t, JPH::ObjectLayer> mStaticLookup;
	std::unordered_map<uint64_t, JPH::ObjectLayer> mMovingLookup;
};

// Object vs object mask test
class LayerPairFilter final : public JPH::ObjectLayerPairFilter
{
public:
	explicit LayerPairFilter(const LayerTable& table) : mTable(table) {}

	bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override
	{
		const LayerTable::Entry& ea = mTable.Get(a);
		const LayerTable::Entry& eb = mTable.Get(b);
		return (ea.Group & eb.Mask) != 0 && (eb.Group & ea.Mask) != 0;
	}

private:
	const LayerTable& mTable;
};

// Static -> Static broad phase, else Moving
class LayerBroadPhaseInterface final : public JPH::BroadPhaseLayerInterface
{
public:
	explicit LayerBroadPhaseInterface(const LayerTable& table) : mTable(table) {}

	JPH::uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::Count; }

	JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override
	{
		return mTable.Get(layer).IsStatic ? BroadPhaseLayers::Static : BroadPhaseLayers::Moving;
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override
	{
		return layer == BroadPhaseLayers::Static ? "Static" : "Moving";
	}
#endif

private:
	const LayerTable& mTable;
};

// Static never tests against Static
class LayerObjectVsBroadPhaseFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	explicit LayerObjectVsBroadPhaseFilter(const LayerTable& table) : mTable(table) {}

	bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer broadPhase) const override
	{
		if (mTable.Get(layer).IsStatic && broadPhase == BroadPhaseLayers::Static) return false;
		return true;
	}

private:
	const LayerTable& mTable;
};
