#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

struct Vertex
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec2 UV = glm::vec2(0.0f);
};

// CCW triangles
struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
};
