#include "Camera3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

static constexpr glm::vec3 kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 Camera3D::Forward() const
{
	float yaw = glm::radians(Yaw);
	float pitch = glm::radians(Pitch);
	return glm::normalize(glm::vec3(
		std::cos(yaw) * std::cos(pitch),
		std::sin(pitch),
		std::sin(yaw) * std::cos(pitch)));
}

glm::vec3 Camera3D::Right() const
{
	return glm::normalize(glm::cross(Forward(), kWorldUp));
}

glm::vec3 Camera3D::Up() const
{
	return glm::normalize(glm::cross(Right(), Forward()));
}

glm::mat4 Camera3D::GetView() const
{
	return glm::lookAt(Position, Position + Forward(), kWorldUp);
}

glm::mat4 Camera3D::GetProjection(float aspect) const
{
	return glm::perspective(glm::radians(FovY), aspect > 0.0f ? aspect : 1.0f, Near, Far);
}

void Camera3D::LookAt(const glm::vec3& target)
{
	glm::vec3 d = target - Position;
	float len = glm::length(d);
	if (len < 1e-6f) return;
	d /= len;
	Pitch = glm::degrees(std::asin(std::clamp(d.y, -1.0f, 1.0f)));
	Yaw = glm::degrees(std::atan2(d.z, d.x));
}
