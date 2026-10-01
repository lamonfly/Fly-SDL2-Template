#pragma once
#include <cstdint>

// Collide when (A.Layer & B.CollidesWith) && (B.Layer & A.CollidesWith)
using CollisionMask = uint32_t;

// Bit n, 0..31
constexpr CollisionMask LayerBit(int n) { return 1u << n; }

// Built-in layers, same indices as Unity. Bits 6..31 free for games.
namespace CollisionLayer
{
	enum : CollisionMask
	{
		Default       = LayerBit(0),
		TransparentFX = LayerBit(1),
		IgnoreRaycast = LayerBit(2),
		Water         = LayerBit(4),
		UI            = LayerBit(5),

		None = 0,
		All  = 0xFFFFFFFFu
	};
}
