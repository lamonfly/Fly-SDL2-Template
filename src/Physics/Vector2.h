#pragma once

#include <format>

class Vector2
{
public:
	int X, Y;
	Vector2(float x = 0, float y = 0) : X(x), Y(y) {};

	// addition
	inline Vector2 operator+(const Vector2& other) const { return Vector2(X + other.X, Y + other.Y); }

	// difference
	inline Vector2 operator-(const Vector2& other) const { return Vector2(X - other.X, Y - other.Y); }

	// multiplication
	inline Vector2 operator*(const Vector2& other) const { return Vector2(X * other.X, Y * other.Y); }

	// string
	inline operator std::string() const { return std::format("({},{})", X, Y); }
};

