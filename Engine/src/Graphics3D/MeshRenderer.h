#pragma once
#include <memory>
#include "Mesh.h"
#include "Material.h"

// Needs Transform3
struct MeshRenderer
{
	std::shared_ptr<::Mesh> Mesh;
	std::shared_ptr<::Material> Material;
	bool Visible = true;
};
