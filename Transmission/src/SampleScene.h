// Base needs
#include "Scene/Scene.h"

// Components
#include "Physics/Transform.h"
#include "Graphics/Sprite.h"
#include <Scene/Camera.h>
#include <Graphics/Line.h>
#include <Graphics/Circle.h>
#include <Event/Grab.h>
#include <Graphics/Text.h>
#include <iostream>

#include "Snake/Snake.h"
#include "Snake/Apple.h"

class SampleScene : public Scene 
{
public:
	void Init() override  
	{
		float currentScale = 20;

		//Snake
		auto snake = mRegistry.create();
		auto &snakeTransform =  mRegistry.emplace<Transform>(snake, Vector2(360, 240));
		snakeTransform.Scale = currentScale;
		mRegistry.emplace<Snake>(snake,  &snakeTransform);

		//Apple
		auto apple = mRegistry.create();
		auto &appleTransform = mRegistry.emplace<Transform>(apple, Vector2(360, 200));
		appleTransform.Scale = currentScale;
		mRegistry.emplace<Apple>(apple);


		//entt::entity foo = mRegistry.create();
		//mRegistry.emplace<Transform>(foo);
		//auto fooTex = new Texture();
		//fooTex->LoadFromFile("res/foo.png", {.r = 0, .g = 0xFF, .b = 0xFF });
		//mRegistry.emplace<Sprite>(foo, fooTex);

		//entt::entity background = mRegistry.create();
		//mRegistry.emplace<Transform>(background);
		//auto backgroundTex = new Texture();
		//backgroundTex->LoadFromFile("res/moss2.png");
		//mRegistry.emplace<Sprite>(background, backgroundTex);

		//entt::entity sampleLine = mRegistry.create();
		//mRegistry.emplace<Transform>(sampleLine);
		//mRegistry.emplace<Line>(sampleLine, Vector2(0, 0), Vector2(100, 100));

		//entt::entity sampleCircle = mRegistry.create();
		//mRegistry.emplace<Circle>(sampleCircle);
		//mRegistry.emplace<Grab>(sampleCircle);
		//mRegistry.emplace<Transform>(sampleCircle).Scale = 4;

		//entt::entity sampleText = mRegistry.create();
		//mRegistry.emplace<Transform>(sampleText);
		//auto textTex = new Texture();
		//textTex->LoadText(TTF_OpenFont("res/Roboto-Regular.ttf", 12), "This a a sample text", {255, 255, 255});
		//mRegistry.emplace<Text>(sampleText, textTex);
	}

	void Render(SDL_Renderer* renderer) override
	{
		RenderType<Snake>(renderer);
		RenderType<Apple>(renderer);
	}

	void Update(double deltaTime) override
	{
		for (auto&& [entityS, transformS, snake] : mRegistry.view<Transform, Snake>().each()) {
			for (auto&& [entityA, transformA, apple] : mRegistry.view<Transform, Apple>().each()) {
				if (transformS.Position.X < transformA.Position.X + transformA.Scale &&
					transformS.Position.X + transformS.Scale > transformA.Position.X &&
					transformS.Position.Y < transformA.Position.Y + transformA.Scale &&
					transformS.Position.Y + transformS.Scale > transformA.Position.Y) 
				{
					snake.EatApple(&transformA);
				}
			}
		}

		UpdateType<Snake>(deltaTime);
	}

	void HandleEvent(SDL_Event& e) override
	{
		HandleEventType<Snake>(e);
	}
};