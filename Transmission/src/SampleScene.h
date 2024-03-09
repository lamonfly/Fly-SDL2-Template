// Base needs
#include "Scene/Scene.h"

// Components
#include "Physics/Transform.h"
#include "Graphics/Sprite.h"
#include <Scene/Camera.h>

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

	void HandleEvent(SDL_Event& e) override
	{

	}
};