#include "Brick.h"

Brick::Brick()
{
	++totalBricks;

	if (totalBricks <= 1) {
		hitSounds.clear();
		hitSounds.push_back(Mix_LoadWAV("res/LedasLuzta.ogg"));
		hitSounds.push_back(Mix_LoadWAV("res/LedasLuzta2.ogg"));
		hitSounds.push_back(Mix_LoadWAV("res/LedasLuzta4.ogg"));
		hitSounds.push_back(Mix_LoadWAV("res/LedasLuzta5.ogg"));
		hitSounds.push_back(Mix_LoadWAV("res/LedasLuzta33.ogg"));
	}
}

Brick::~Brick()
{
	--totalBricks;

	if (totalBricks <= 0) {
		for (auto sound : Brick::hitSounds) {
			Mix_FreeChunk(sound);
		}
	}
}

void Brick::TakeDamage()
{
	static int volume = (MIX_MAX_VOLUME * 60) / 100;

	Health--;
	Mix_Volume(-1, volume);
	Mix_PlayChannel(-1, hitSounds[(rand() % hitSounds.size())], 0);
}
