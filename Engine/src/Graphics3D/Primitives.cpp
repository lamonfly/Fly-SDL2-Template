#include "Primitives.h"

#include <glm/gtc/constants.hpp>
#include <cmath>

namespace
{
	// u x v = n, CCW from outside
	void AddFace(MeshData& out, const glm::vec3& n, const glm::vec3& u, const glm::vec3& v, const glm::vec3& half)
	{
		uint32_t base = static_cast<uint32_t>(out.Vertices.size());
		glm::vec3 c = n * half;
		glm::vec3 du = u * half;
		glm::vec3 dv = v * half;

		out.Vertices.push_back({ c - du - dv, n, { 0.0f, 0.0f } });
		out.Vertices.push_back({ c + du - dv, n, { 1.0f, 0.0f } });
		out.Vertices.push_back({ c + du + dv, n, { 1.0f, 1.0f } });
		out.Vertices.push_back({ c - du + dv, n, { 0.0f, 1.0f } });

		out.Indices.insert(out.Indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });
	}

	// Strip between two rings of (segments + 1) vertices
	void AddRingStrip(MeshData& out, uint32_t top, uint32_t bottom, int segments)
	{
		for (int s = 0; s < segments; s++)
		{
			uint32_t a = top + s;
			uint32_t b = bottom + s;
			out.Indices.insert(out.Indices.end(), { a, a + 1, b + 1, a, b + 1, b });
		}
	}

	// Latitude ring. phi 0 = top
	uint32_t AddRing(MeshData& out, float radius, float phi, float yOffset, float v, int segments)
	{
		uint32_t start = static_cast<uint32_t>(out.Vertices.size());
		float sp = std::sin(phi);
		float cp = std::cos(phi);
		for (int s = 0; s <= segments; s++)
		{
			float theta = glm::two_pi<float>() * static_cast<float>(s) / static_cast<float>(segments);
			glm::vec3 n(sp * std::cos(theta), cp, sp * std::sin(theta));
			out.Vertices.push_back({ n * radius + glm::vec3(0.0f, yOffset, 0.0f), n, { static_cast<float>(s) / segments, v } });
		}
		return start;
	}
}

namespace Primitives
{
	MeshData Cube(float size)
	{
		return Box(size, size, size);
	}

	MeshData Box(float width, float height, float depth)
	{
		MeshData out;
		glm::vec3 half(width * 0.5f, height * 0.5f, depth * 0.5f);
		const glm::vec3 X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1);
		AddFace(out, Z, X, Y, half);
		AddFace(out, -Z, -X, Y, half);
		AddFace(out, X, -Z, Y, half);
		AddFace(out, -X, Z, Y, half);
		AddFace(out, Y, X, -Z, half);
		AddFace(out, -Y, X, Z, half);
		return out;
	}

	MeshData Sphere(float radius, int segments, int rings)
	{
		MeshData out;
		if (segments < 3) segments = 3;
		if (rings < 2) rings = 2;

		for (int r = 0; r <= rings; r++)
		{
			float phi = glm::pi<float>() * static_cast<float>(r) / static_cast<float>(rings);
			AddRing(out, radius, phi, 0.0f, 1.0f - static_cast<float>(r) / rings, segments);
		}

		uint32_t stride = static_cast<uint32_t>(segments + 1);
		for (int r = 0; r < rings; r++)
			AddRingStrip(out, r * stride, (r + 1) * stride, segments);

		return out;
	}

	MeshData Plane(float width, float depth, float uvRepeat)
	{
		MeshData out;
		float hw = width * 0.5f;
		float hd = depth * 0.5f;
		glm::vec3 n(0.0f, 1.0f, 0.0f);
		out.Vertices.push_back({ { -hw, 0.0f, -hd }, n, { 0.0f, 0.0f } });
		out.Vertices.push_back({ { -hw, 0.0f,  hd }, n, { 0.0f, uvRepeat } });
		out.Vertices.push_back({ {  hw, 0.0f,  hd }, n, { uvRepeat, uvRepeat } });
		out.Vertices.push_back({ {  hw, 0.0f, -hd }, n, { uvRepeat, 0.0f } });
		out.Indices = { 0, 1, 2, 0, 2, 3 };
		return out;
	}

	MeshData Capsule(float radius, float halfHeight, int segments, int rings)
	{
		MeshData out;
		if (segments < 3) segments = 3;
		if (rings < 2) rings = 2;
		if (rings % 2) rings++;

		int half = rings / 2;
		float total = 2.0f * radius + 2.0f * halfHeight;

		// Top hemisphere, +halfHeight
		for (int r = 0; r <= half; r++)
		{
			float phi = glm::pi<float>() * static_cast<float>(r) / static_cast<float>(rings);
			float y = radius * std::cos(phi) + halfHeight;
			AddRing(out, radius, phi, halfHeight, (y + radius + halfHeight) / total, segments);
		}

		// Bottom hemisphere, -halfHeight
		for (int r = half; r <= rings; r++)
		{
			float phi = glm::pi<float>() * static_cast<float>(r) / static_cast<float>(rings);
			float y = radius * std::cos(phi) - halfHeight;
			AddRing(out, radius, phi, -halfHeight, (y + radius + halfHeight) / total, segments);
		}

		uint32_t stride = static_cast<uint32_t>(segments + 1);
		int ringCount = rings + 2;
		for (int r = 0; r < ringCount - 1; r++)
			AddRingStrip(out, r * stride, (r + 1) * stride, segments);

		return out;
	}

	std::shared_ptr<Mesh> Make(const MeshData& data)
	{
		return std::make_shared<Mesh>(data);
	}
}
