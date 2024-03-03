// Base needs
#include "Scene.h"

// Components
#include "Physics/Transform.h"
#include "Graphics/Sprite.h"

class SampleScene : public Scene 
{
public:
	void Init() override 
	{
		//Create components
		entt::entity foo = mRegistry.create();
		mRegistry.emplace<Transform>(foo);
		auto fooTex = new Texture();
		fooTex->LoadFromFile("res/foo.png");
		mRegistry.emplace<Sprite>(foo, fooTex);

		entt::entity background = mRegistry.create();
		mRegistry.emplace<Transform>(background);
		auto backgroundTex = new Texture();
		backgroundTex->LoadFromFile("res/moss2.png");
		mRegistry.emplace<Sprite>(background, backgroundTex);
	}

	void Update() override
	{

	}

	void Render(SDL_Renderer* renderer) override
	{
		//Render sprite
		auto view = mRegistry.view<Transform, Sprite>();
		for (auto entity : view)
		{
			auto [transform, sprite] = view.get(entity);

			sprite.Render(transform);
		}
	}

	void HandleEvent(SDL_Event& e) override
	{

	}
};