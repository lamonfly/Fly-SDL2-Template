#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <memory>
#include <string>

// GL 2D texture, RGBA8, mipmapped
class TextureGL
{
public:
	// channels 1, 3 or 4
	static std::shared_ptr<TextureGL> FromPixels(int width, int height, int channels, const uint8_t* pixels);

	// stb_image, forced RGBA. Null on failure
	static std::shared_ptr<TextureGL> FromFile(const std::string& path);

	~TextureGL();
	TextureGL(const TextureGL&) = delete;
	TextureGL& operator=(const TextureGL&) = delete;

	void Bind(int unit) const;
	GLuint Id() const { return mId; }
	int Width() const { return mWidth; }
	int Height() const { return mHeight; }

private:
	TextureGL() = default;

	GLuint mId = 0;
	int mWidth = 0;
	int mHeight = 0;
};
