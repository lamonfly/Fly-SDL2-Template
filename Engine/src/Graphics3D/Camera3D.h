#pragma once
#include <glm/glm.hpp>

// Perspective camera, degrees. Yaw -90 looks down -Z
class Camera3D
{
public:
	glm::vec3 Position = glm::vec3(0.0f, 2.0f, 6.0f);
	float Yaw = -90.0f;
	float Pitch = 0.0f;
	float FovY = 60.0f;
	float Near = 0.1f;
	float Far = 500.0f;

	glm::vec3 Forward() const;
	glm::vec3 Right() const;
	glm::vec3 Up() const;

	glm::mat4 GetView() const;
	glm::mat4 GetProjection(float aspect) const;

	void LookAt(const glm::vec3& target);
};
