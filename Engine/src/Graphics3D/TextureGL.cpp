#include "TextureGL.h"

#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include <stb_image.h>
#pragma warning(pop)

#include <cstdio>

std::shared_ptr<TextureGL> TextureGL::FromPixels(int width, int height, int channels, const uint8_t* pixels)
{
	GLenum format = GL_RGBA;
	switch (channels)
	{
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	default: format = GL_RGBA; break;
	}

	std::shared_ptr<TextureGL> tex(new TextureGL());
	tex->mWidth = width;
	tex->mHeight = height;

	glGenTextures(1, &tex->mId);
	glBindTexture(GL_TEXTURE_2D, tex->mId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

std::shared_ptr<TextureGL> TextureGL::FromFile(const std::string& path)
{
	int w = 0, h = 0, n = 0;
	stbi_set_flip_vertically_on_load(0);
	stbi_uc* data = stbi_load(path.c_str(), &w, &h, &n, 4);
	if (!data)
	{
		printf("TextureGL: failed to load %s: %s\n", path.c_str(), stbi_failure_reason());
		return nullptr;
	}

	std::shared_ptr<TextureGL> tex = FromPixels(w, h, 4, data);
	stbi_image_free(data);
	return tex;
}

TextureGL::~TextureGL()
{
	if (mId) glDeleteTextures(1, &mId);
}

void TextureGL::Bind(int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, mId);
}
