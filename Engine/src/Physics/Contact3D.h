#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "Contact.h"

struct Contact3D
{
	entt::entity Other = entt::null;
	glm::vec3 Point = glm::vec3(0.0f);   // meters
	glm::vec3 Normal = glm::vec3(0.0f);  // towards Other
	ContactPhase Phase = ContactPhase::Enter;
};
