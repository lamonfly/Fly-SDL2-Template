#pragma once
#include "Scene/Scene.h"
#include <Graphics/Line.h>

class BackgroundScene : public Scene
{
public:
	void Init() override
	{
		entt::entity leftLine = mRegistry.create();
		mRegistry.emplace<Transform>(leftLine);
		auto& left = mRegistry.emplace<Line>(leftLine, Vector2(0, 0), Vector2(0, 480));
		left.Color = { 255,255,255,255 };

		entt::entity topLine = mRegistry.create();
		mRegistry.emplace<Transform>(topLine);
		auto& top = mRegistry.emplace<Line>(topLine, Vector2(0, 0), Vector2(720, 0));
		top.Color = { 255,255,255,255 };

		entt::entity rightLine = mRegistry.create();
		mRegistry.emplace<Transform>(rightLine);
		auto& right = mRegistry.emplace<Line>(rightLine, Vector2(719, 0), Vector2(719, 480));
		right.Color = { 255,255,255,255 };

		entt::entity bottomLine = mRegistry.create();
		mRegistry.emplace<Transform>(bottomLine);
		auto& bottom = mRegistry.emplace<Line>(bottomLine, Vector2(0, 479), Vector2(720, 479));
		bottom.Color = { 255,255,255,255 };
	}

	void Render(SDL_Renderer* renderer) override
	{
		RenderType<Line>(renderer);
	}

	void Update(double deltaTime) override
	{

	}

	void HandleEvent(SDL_Event& e) override
	{
	}
};