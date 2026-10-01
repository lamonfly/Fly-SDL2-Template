#pragma once
#include <SDL_mixer.h>
#include <vector>

struct Brick
{
	inline static int totalBricks;

	// Audio
	inline static int volume = (MIX_MAX_VOLUME * 60) / 100;
	inline static std::vector<Mix_Chunk*> hitSounds = {};
public:
	int Health = 1;

	Brick();
	~Brick();

	void TakeDamage();
};