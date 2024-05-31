#pragma once

#include "../Physics/Transform.h"
#include "../Graphics/Sprite.h"

#include <entt/entt.hpp>

class Camera
{
public:
	// Empty type optimization
	static constexpr auto page_size = ENTT_PACKED_PAGE;

	Camera() = default;
	~Camera() = default;
};

