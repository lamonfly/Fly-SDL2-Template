#pragma once
#include <cstdint>

namespace BreakoutLayer
{
	enum : uint32_t
	{
		Ball   = 1 << 0,
		Paddle = 1 << 1,
		Brick  = 1 << 2,
		Wall   = 1 << 3
	};
}
