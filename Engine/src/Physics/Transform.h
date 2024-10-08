#pragma once
#include "Vector2.h"

struct Transform
{
private:
	double rotation = 0.0f;

public:
	float Scale = 1;
	Vector2 Size = { 1, 1 };
	Vector2 Position;

	Transform() = default;

	Transform(Vector2 vector)
	{
		Position = vector;
	}

	inline double getRotation()
	{
		return rotation;
	}

	inline void setRotation(double newRotation) {
		newRotation = fmod(newRotation + 180, 360);
		if (newRotation < 0)
			rotation += 360;
	}

	inline Transform operator+(Transform const& other)
	{
		Transform res;
		res.setRotation(other.rotation + rotation);
		auto rotatedPosition = Vector2(
			((other.Position.X - Position.X) * (float)cos(rotation)) - ((Position.Y - other.Position.Y) * (float)sin(rotation)) + Position.X,
			Position.Y - ((Position.Y - other.Position.Y) * (float)cos(rotation)) + ((other.Position.X - Position.X) * (float)sin(rotation))
			);
		res.Position = rotatedPosition + Position;

		return res;
	}
};
