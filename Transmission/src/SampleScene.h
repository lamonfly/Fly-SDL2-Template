// Base needs
#include "Scene/Scene.h"

// Components
#include "Physics/Transform.h"
#include "Graphics/Sprite.h"
#include <Scene/Camera.h>
#include <Graphics/Line.h>
#include <Graphics/Circle.h>
#include <Event/Grab.h>

class SampleScene : public Scene 
{
public:
	void Init() override  
	{
		auto camera = mRegistry.create();
		mRegistry.emplace<Transform>(camera);
		mRegistry.emplace<Camera>(camera);

		//Create components
		entt::entity foo = mRegistry.create();
		mRegistry.emplace<Transform>(foo);
		auto fooTex = new Texture();
		fooTex->LoadFromFile("res/foo.png", {.r = 0, .g = 0xFF, .b = 0xFF });
		mRegistry.emplace<Sprite>(foo, fooTex);

		entt::entity background = mRegistry.create();
		mRegistry.emplace<Transform>(background);
		auto backgroundTex = new Texture();
		backgroundTex->LoadFromFile("res/moss2.png");
		mRegistry.emplace<Sprite>(background, backgroundTex);

		entt::entity sampleLine = mRegistry.create();
		mRegistry.emplace<Transform>(sampleLine);
		mRegistry.emplace<Line>(sampleLine, Vector2(0, 0), Vector2(100, 100));

		entt::entity sampleCircle = mRegistry.create();
		mRegistry.emplace<Transform>(sampleCircle);
		mRegistry.emplace<Circle>(sampleCircle, 4);
		mRegistry.emplace<Grab>(sampleCircle);
	}

	void Render(SDL_Renderer* renderer) override
	{
		RenderType<Sprite>(renderer);
		RenderType<Line>(renderer);
		RenderType<Circle>(renderer);
	}

	void Update() override
	{

	}

	void HandleEvent(SDL_Event& e) override
	{
		for (auto&& [entity, transform, grab] : mRegistry.view<Transform, Grab>().each()) {
			grab.HandleEvent(e, transform);
		}
	}
};