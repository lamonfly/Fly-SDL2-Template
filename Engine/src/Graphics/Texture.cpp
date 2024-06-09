#include "Texture.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

Texture::Texture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

Texture::~Texture()
{
	//Deallocate
	Free();
}

bool Texture::LoadFromFile(std::string path)
{
	//Get rid of preexisting texture
	Free();

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(GetRenderer(), loadedSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	return mTexture != NULL;
}

bool Texture::LoadFromFile(std::string path, SDL_Color colorKey)
{
	//Get rid of preexisting texture
	Free();

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorKey.r, colorKey.g, colorKey.b));

		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(GetRenderer(), loadedSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	return mTexture != NULL;
}

bool Texture::LoadText(TTF_Font* font, std::string text, SDL_Color color)
{
	//Get rid of preexisting texture
	Free();

	//Load text
	SDL_Surface* loadedSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	if (loadedSurface == NULL)
	{
		printf("Unable to load text! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(GetRenderer(), loadedSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	return mTexture != NULL;
}

void Texture::Free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::SetColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void Texture::SetBlendMode(SDL_BlendMode blending)
{
	//Set blending mode
	SDL_SetTextureBlendMode(mTexture, blending);
}

void Texture::SetAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void Texture::Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void Texture::Render(SDL_Renderer* renderer, float x, float y, SDL_Rect* clip, double angle, SDL_FPoint* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_FRect renderQuad = { x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = static_cast<float>(clip->w);
		renderQuad.h = static_cast<float>(clip->h);
	}

	SDL_RenderCopyExF(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}