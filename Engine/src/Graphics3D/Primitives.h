#pragma once
#include <memory>
#include "Vertex.h"
#include "Mesh.h"

// Sizes match Jolt shapes: Cube(size) = Box(size), Sphere(r), Capsule(r, halfHeight)
namespace Primitives
{
	MeshData Cube(float size = 1.0f);
	MeshData Box(float width, float height, float depth);
	MeshData Sphere(float radius = 0.5f, int segments = 32, int rings = 16);
	MeshData Plane(float width = 1.0f, float depth = 1.0f, float uvRepeat = 1.0f);
	MeshData Capsule(float radius = 0.5f, float halfHeight = 0.5f, int segments = 16, int rings = 8);

	std::shared_ptr<Mesh> Make(const MeshData& data);
}
