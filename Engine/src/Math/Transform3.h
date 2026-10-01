#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Meters, Y up, right-handed, -Z forward. Position = center
struct Transform3
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // w, x, y, z
	glm::vec3 Scale = glm::vec3(1.0f);

	Transform3() = default;
	explicit Transform3(const glm::vec3& position) : Position(position) {}
	Transform3(const glm::vec3& position, const glm::vec3& scale) : Position(position), Scale(scale) {}

	glm::mat4 ToMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), Position) * glm::mat4_cast(Rotation) * glm::scale(glm::mat4(1.0f), Scale);
	}

	static Transform3 FromMatrix(const glm::mat4& m)
	{
		Transform3 t;
		t.Position = glm::vec3(m[3]);
		t.Scale = glm::vec3(glm::length(glm::vec3(m[0])), glm::length(glm::vec3(m[1])), glm::length(glm::vec3(m[2])));
		glm::mat3 rot(
			glm::vec3(m[0]) / t.Scale.x,
			glm::vec3(m[1]) / t.Scale.y,
			glm::vec3(m[2]) / t.Scale.z);
		t.Rotation = glm::normalize(glm::quat_cast(rot));
		return t;
	}

	glm::vec3 Forward() const { return Rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
	glm::vec3 Right() const { return Rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
	glm::vec3 Up() const { return Rotation * glm::vec3(0.0f, 1.0f, 0.0f); }
};
