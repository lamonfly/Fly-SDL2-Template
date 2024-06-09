#pragma once

#include <format>
#include <cmath>

class Vector2
{
public:
	float X, Y;
	Vector2(float x = 0, float y = 0) : X(x), Y(y) {};

	// addition
	inline Vector2 operator+(const Vector2& other) const { return Vector2(X + other.X, Y + other.Y); }

	// difference
	inline Vector2 operator-(const Vector2& other) const { return Vector2(X - other.X, Y - other.Y); }

	// multiplication
	inline Vector2 operator*(const Vector2& other) const { return Vector2(X * other.X, Y * other.Y); }

	// string
	inline operator std::string() const { return std::format("({},{})", X, Y); }

	// distance
	inline float Distance(Vector2 other)
	{
		return sqrtf(powf(X - other.X, 2) + powf(Y - other.Y, 2));
	};
};
