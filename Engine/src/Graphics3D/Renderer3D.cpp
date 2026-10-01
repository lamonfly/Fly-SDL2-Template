#include "Renderer3D.h"
#include "BasicShaders.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <cstdio>
#include <cstring>

#pragma comment(lib, "opengl32.lib")

#ifdef GLAD_DEBUG
static void GladPostCallback(const char* name, void* funcptr, int lenArgs, ...)
{
	if (std::strcmp(name, "glGetError") == 0) return;
	GLenum err = glad_glGetError();
	if (err != GL_NO_ERROR)
		printf("GL error 0x%04X in %s\n", err, name);
}
#endif

bool Renderer3D::Init()
{
#ifdef GLAD_DEBUG
	glad_set_post_callback(GladPostCallback);
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	mCullEnabled = true;

	mShader = Shader::FromSource(BasicShaders::Vertex, BasicShaders::Fragment);
	if (!mShader) return false;

	const uint8_t white[4] = { 255, 255, 255, 255 };
	mWhite = TextureGL::FromPixels(1, 1, 4, white);

	return true;
}

void Renderer3D::SetViewport(int width, int height)
{
	glViewport(0, 0, width, height);
	mAspect = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
}

void Renderer3D::Clear(SDL_Color color)
{
	glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer3D::BeginFrame(const Camera3D& camera)
{
	mView = camera.GetView();
	mProj = camera.GetProjection(mAspect);
	mViewPos = camera.Position;

	mShader->Bind();
	mShader->SetMat4("uView", mView);
	mShader->SetMat4("uProj", mProj);
	mShader->SetVec3("uViewPos", mViewPos);
	mShader->SetVec3("uLightDir", glm::normalize(Light.Direction));
	mShader->SetVec3("uLightColor", Light.Color);
	mShader->SetVec3("uAmbient", Light.Ambient);
	mShader->SetInt("uBaseColorTex", 0);
}

void Renderer3D::Draw(const Mesh& mesh, const Material& material, const glm::mat4& model)
{
	mShader->SetMat4("uModel", model);
	mShader->SetMat3("uNormalMatrix", glm::inverseTranspose(glm::mat3(model)));
	mShader->SetVec4("uBaseColor", material.BaseColor);

	const TextureGL& tex = material.BaseColorTexture ? *material.BaseColorTexture : *mWhite;
	tex.Bind(0);

	bool cull = !material.DoubleSided;
	if (cull != mCullEnabled)
	{
		if (cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		mCullEnabled = cull;
	}

	mesh.Draw();
}

void Renderer3D::EndFrame()
{
	glBindVertexArray(0);
	glUseProgram(0);
}
