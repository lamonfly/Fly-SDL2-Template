#pragma once

#include <format>
#include <cmath>
#include <string>

struct Vector2
{
public:
	float X, Y;
	Vector2(float x = 0, float y = 0) : X(x), Y(y) {};

	// addition
	inline Vector2 operator+(const Vector2& other) const { return Vector2(X + other.X, Y + other.Y); }

	inline Vector2& operator+=(const Vector2& other) 
	{ 
		X += other.X;
		Y += other.Y;
		return *this; 
	}

	// difference
	inline Vector2 operator-(const Vector2& other) const { return Vector2(X - other.X, Y - other.Y); }

	// multiplication
	inline Vector2 operator*(const Vector2& other) const { return Vector2(X * other.X, Y * other.Y); }

	inline Vector2 operator*(const float& other) const { return Vector2(X * other, Y * other); }

	// division
	inline Vector2 operator/(const int& other) const { return Vector2(X / other, Y / other); }

	// string
	inline operator std::string() const { return std::format("({},{})", X, Y); }

	// distance
	inline float Distance(Vector2 other)
	{
		return sqrtf(powf(X - other.X, 2) + powf(Y - other.Y, 2));
	};

	// magnitude
	inline float Magnitude() const {
		return sqrt(X * X + Y * Y);
	}

	// normalize
	inline Vector2 Normalized() {
		float mag = Magnitude();
		if (mag != 0) {
			return Vector2(X / mag, Y / mag);
		}
		else {
			return Vector2(0, 0);
		}
	}

	// dot
	inline float Dot(Vector2 other) {
		return X * other.X + Y * other.Y;
	}

	inline std::string ToString() {
		return std::format("({},{})", X, Y);
	}

	// Given three collinear points the function checks if 
	// Vector2 lies on line segment 'pr' 
	inline bool OnSegment(Vector2 q, Vector2 r) {
		if (q.X <= fmax(X, r.X) && q.X >= fmin(X, r.X) &&
			q.Y <= fmax(Y, r.Y) && q.Y >= fmin(Y, r.Y))
			return true;

		return false;
	}

	// To find orientation of ordered triplet
	// The function returns following values 
	// 0 --> p, q and r are collinear 
	// 1 --> Clockwise 
	// 2 --> Counterclockwise 
	inline int Orientation(Vector2 q, Vector2 r) {
		int val = static_cast<int>((q.Y - Y) * (r.X - q.X) -
			(q.X - X) * (r.Y - q.Y));

		if (val == 0) return 0;  // collinear 

		return (val > 0) ? 1 : 2; // clock or counterclock wise 
	}
};
