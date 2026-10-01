#pragma once
#include <glad/glad.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <memory>

#include "Camera3D.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "TextureGL.h"

struct DirectionalLight
{
	glm::vec3 Direction = glm::vec3(-0.4f, -1.0f, -0.3f);
	glm::vec3 Color = glm::vec3(1.0f);
	glm::vec3 Ambient = glm::vec3(0.15f);
};

// Forward renderer, one shader, one light
class Renderer3D
{
public:
	Renderer3D() = default;
	~Renderer3D() = default;

	Renderer3D(const Renderer3D&) = delete;
	Renderer3D& operator=(const Renderer3D&) = delete;

	// Needs current GL context
	bool Init();

	void SetViewport(int width, int height);
	void Clear(SDL_Color color);

	void BeginFrame(const Camera3D& camera);
	void Draw(const Mesh& mesh, const Material& material, const glm::mat4& model);
	void EndFrame();

	DirectionalLight Light;

	float Aspect() const { return mAspect; }
	const glm::mat4& View() const { return mView; }
	const glm::mat4& Projection() const { return mProj; }

private:
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<TextureGL> mWhite;
	glm::mat4 mView = glm::mat4(1.0f);
	glm::mat4 mProj = glm::mat4(1.0f);
	glm::vec3 mViewPos = glm::vec3(0.0f);
	float mAspect = 1.0f;
	bool mCullEnabled = true;
};
