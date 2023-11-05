#pragma once

#include <string>
#include <SDL.h>

//Texture wrapper class
class Texture
{
private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//SDL renderer used for images
	static SDL_Renderer* sRenderer;

	//Image dimensions
	int mWidth;
	int mHeight;

public:
	//Initializes variables
	Texture();

	//Deallocates memory
	~Texture();

	//Loads image at specified path
	bool LoadFromFile(std::string path);

	//Deallocates texture
	void Free();

	//Set color modulation
	void SetColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void SetBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void SetAlpha(Uint8 alpha);

	//Renders texture at given point
	void Render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	void Render(float x, float y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Get image dimensions
	inline int GetWidth() { return mWidth; }
	inline int GetHeight() { return mHeight; }

	//Set renderer
	inline static void SetRenderer(SDL_Renderer* renderer) { sRenderer = renderer; }
};

