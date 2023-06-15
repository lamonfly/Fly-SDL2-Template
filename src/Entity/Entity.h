#pragma once

#include "Component.h"
#include <map>

// Collection of components
class Entity {
public:
	Entity() {}
	virtual ~Entity() {}

	std::map<type_info, Component> Components;
};