#pragma once

struct BallMovement
{
	float Speed = 150.0f;      // px/s
	float SpinDegPerSec = 100.0f;
	bool Launched = false;

	void OnBounce() { Speed *= 1.01f; }
};
