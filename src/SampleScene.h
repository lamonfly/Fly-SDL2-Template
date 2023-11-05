// Base needs
#include "Scene.h"

// Components
#include "Physics/Transform.h"
#include "Graphics/Sprite.h"

class SampleScene : Scene 
{
public:
	bool Init(entt::registry* registry) override
	{
		//Create components
		entt::entity foo = registry->create();
		registry->emplace<Transform>(foo);
		auto fooTex = new Texture();
		fooTex->LoadFromFile("res/foo.png");
		registry->emplace<Sprite>(foo, fooTex);

		entt::entity background = registry->create();
		registry->emplace<Transform>(background);
		auto backgroundTex = new Texture();
		backgroundTex->LoadFromFile("res/moss2.png");
		registry->emplace<Sprite>(background, backgroundTex);

		return true;
	};
};