#pragma once
#include "Scene/Scene3D.h"
#include "Core/Engine.h"
#include "Graphics3D/Renderer3D.h"
#include "Graphics3D/MeshRenderer.h"
#include "Graphics3D/Primitives.h"
#include "Graphics3D/AssetCache.h"
#include "FlyCamera.h"

#include <Jolt/Physics/Body/BodyManager.h>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <limits>

// Boxes and spheres on a floor, fly camera
class SampleScene3D : public Scene3D
{
public:
	void Init() override
	{
		SetPhysicsConfig3D(PhysicsConfig3D{});

		mCube = mAssets.GetMesh("cube", [] { return Primitives::Cube(1.0f); });
		mSphere = mAssets.GetMesh("sphere", [] { return Primitives::Sphere(1.0f, 32, 16); });

		mFloorMat = std::make_shared<Material>();
		mFloorMat->BaseColor = glm::vec4(0.55f, 0.58f, 0.62f, 1.0f);

		// Floor and walls
		SpawnStatic(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(20.0f, 1.0f, 20.0f));
		SpawnStatic(glm::vec3(10.25f, 0.5f, 0.0f), glm::vec3(0.5f, 1.0f, 20.5f));
		SpawnStatic(glm::vec3(-10.25f, 0.5f, 0.0f), glm::vec3(0.5f, 1.0f, 20.5f));
		SpawnStatic(glm::vec3(0.0f, 0.5f, 10.25f), glm::vec3(20.0f, 1.0f, 0.5f));
		SpawnStatic(glm::vec3(0.0f, 0.5f, -10.25f), glm::vec3(20.0f, 1.0f, 0.5f));

		// Grid of boxes and spheres
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++)
				for (int z = 0; z < 4; z++)
				{
					glm::vec3 pos((x - 1.5f) * 1.2f + Jitter(), 4.0f + y * 1.2f, (z - 1.5f) * 1.2f + Jitter());
					bool box = ((x + y + z) % 2) == 0;
					if (box) SpawnBox(pos, glm::vec3(0.5f));
					else SpawnSphere(pos, 0.3f, glm::vec3(0.0f));
				}

		// Optional glTF
		const char* modelPath = "res/model.glb";
		if (std::filesystem::exists(modelPath))
		if (const GltfModel* model = mAssets.GetModel(modelPath))
		{
			for (const GltfModel::Instance& inst : model->Instances)
			{
				Transform3 t = Transform3::FromMatrix(inst.World);
				t.Position += glm::vec3(0.0f, 0.0f, -6.0f);
				auto e = mRegistry.create();
				mRegistry.emplace<Transform3>(e, t);
				mRegistry.emplace<MeshRenderer>(e, model->Meshes[inst.MeshIndex], model->Materials[inst.MeshIndex]);
			}
		}

		mFly.Apply(Camera);
	}

	void Update(double deltaTime) override
	{
		mFly.Update(static_cast<float>(deltaTime), SDL_GetKeyboardState(nullptr));
		mFly.Apply(Camera);

		mLogTimer += deltaTime;
		if (mLogTimer >= 1.0)
		{
			mLogTimer = 0.0;
			float minY = std::numeric_limits<float>::max();
			int dynamic = 0;
			for (auto&& [e, t, body] : mRegistry.view<Transform3, RigidBody3D>().each())
			{
				if (body.Type != BodyType::Dynamic) continue;
				dynamic++;
				minY = std::min(minY, t.Position.y);
			}
			unsigned active = GetPhysics3D().GetSystem().GetNumActiveBodies(JPH::EBodyType::RigidBody);
			printf("active=%u dynamic=%d minY=%.2f\n", active, dynamic, dynamic ? minY : 0.0f);
			fflush(stdout);
		}
	}

	void Render3D(Renderer3D& renderer) override
	{
		renderer.Light.Direction = glm::vec3(-0.4f, -1.0f, -0.3f);
		RenderMeshes(renderer);
	}

	void HandleEvent(SDL_Event& e) override
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !mFly.Captured)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
			mFly.Captured = true;
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			mFly.OnMouseMotion(e.motion.xrel, e.motion.yrel);
		}
		else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				if (mFly.Captured)
				{
					SDL_SetRelativeMouseMode(SDL_FALSE);
					mFly.Captured = false;
				}
				else
				{
					Engine::GetInstance()->Quit();
				}
				break;

			case SDLK_SPACE:
				SpawnSphere(mFly.Position + mFly.Forward() * 1.0f, 0.3f, mFly.Forward() * 15.0f);
				break;
			}
		}
	}

private:
	static float Jitter() { return (rand() % 1000) / 1000.0f * 0.2f - 0.1f; }

	void SpawnStatic(const glm::vec3& pos, const glm::vec3& size)
	{
		auto e = mRegistry.create();
		mRegistry.emplace<Transform3>(e, pos, size);
		mRegistry.emplace<RigidBody3D>(e, RigidBody3D::Box(size, BodyType::Static));
		mRegistry.emplace<MeshRenderer>(e, mCube, mFloorMat);
	}

	std::shared_ptr<Material> RandomMaterial()
	{
		auto mat = std::make_shared<Material>();
		mat->BaseColor = glm::vec4(0.3f + (rand() % 70) / 100.0f, 0.3f + (rand() % 70) / 100.0f, 0.3f + (rand() % 70) / 100.0f, 1.0f);
		return mat;
	}

	void SpawnBox(const glm::vec3& pos, const glm::vec3& size)
	{
		auto e = mRegistry.create();
		mRegistry.emplace<Transform3>(e, pos, size);
		auto body = RigidBody3D::Box(size, BodyType::Dynamic);
		body.Restitution = 0.3f;
		mRegistry.emplace<RigidBody3D>(e, body);
		mRegistry.emplace<MeshRenderer>(e, mCube, RandomMaterial());
	}

	void SpawnSphere(const glm::vec3& pos, float radius, const glm::vec3& velocity)
	{
		auto e = mRegistry.create();
		mRegistry.emplace<Transform3>(e, pos, glm::vec3(radius));
		auto body = RigidBody3D::Sphere(radius, BodyType::Dynamic);
		body.Restitution = 0.3f;
		auto& rb = mRegistry.emplace<RigidBody3D>(e, body);
		GetPhysics3D().SetLinearVelocity(rb, velocity);
		mRegistry.emplace<MeshRenderer>(e, mSphere, RandomMaterial());
	}

	AssetCache mAssets;
	std::shared_ptr<Mesh> mCube;
	std::shared_ptr<Mesh> mSphere;
	std::shared_ptr<Material> mFloorMat;
	FlyCamera mFly;
	double mLogTimer = 0.0;
};
