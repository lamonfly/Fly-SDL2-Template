#pragma once
#include <entt/entt.hpp>
#include "Vector2.h"

enum class ContactPhase
{
	Enter,
	Stay,
	Exit    // Point and Normal are zero
};

struct Contact
{
	entt::entity Other = entt::null;
	Vector2 Point;   // pixels
	Vector2 Normal;  // towards Other
	ContactPhase Phase = ContactPhase::Enter;
};
