#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Material.h"

// First TRIANGLES primitive per glTF mesh
struct GltfModel
{
	struct Instance
	{
		int MeshIndex = -1;
		glm::mat4 World = glm::mat4(1.0f);
	};

	std::vector<std::shared_ptr<Mesh>> Meshes;
	std::vector<std::shared_ptr<Material>> Materials;   // index aligned with Meshes
	std::vector<Instance> Instances;                    // default scene nodes
};

class GltfLoader
{
public:
	// .gltf or .glb. Needs current GL context
	static std::optional<GltfModel> Load(const std::string& path);
};
