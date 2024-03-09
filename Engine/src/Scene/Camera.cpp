#include "Camera.h"

void Camera::RenderSprite(Transform transform, std::tuple<Transform&, Sprite&> tuple)
{
	auto offsetPosition = transform + std::get<Transform&>(tuple);
	std::get<Sprite&>(tuple).Render(offsetPosition);
}