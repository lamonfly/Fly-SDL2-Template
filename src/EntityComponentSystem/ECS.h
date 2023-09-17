#pragma once
#include <bitset>
#include <array>
#include <vector>
#include <memory>

class Component;
class Entity;

using ComponentID = std::size_t;

inline ComponentID getComponentTypeID() 
{
	static ComponentID lastID = 0;
	return lastID++;
}

template <typename T> inline ComponentID getComponentTypeID() noexcept
{
	static ComponentID typeID = getComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;
