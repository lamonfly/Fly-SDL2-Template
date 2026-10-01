#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "TextureGL.h"

struct Material
{
	glm::vec4 BaseColor = glm::vec4(1.0f);
	std::shared_ptr<TextureGL> BaseColorTexture;   // null = white
	bool DoubleSided = false;
};
