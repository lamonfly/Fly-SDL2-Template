// Base needs
#include "Scene/Scene.h"

// Components
#include <Physics/Transform.h>
#include <Graphics/Text.h>

class GameOverScene : public Scene
{
public:

	void Init() override
	{
		int windowHeight = Engine::GetInstance()->GetWindow()->GetResolutionHeight();
		int windowWidth = Engine::GetInstance()->GetWindow()->GetResolutionWidth();

		entt::entity sampleText = mRegistry.create();
		auto textTex = new Texture();
		textTex->LoadText(TTF_OpenFont("res/Roboto-Regular.ttf", 24), "Game over!", {0, 0, 0});
		mRegistry.emplace<Text>(sampleText, textTex);
		mRegistry.emplace<Transform>(sampleText, Vector2((windowWidth - textTex->GetWidth()) / 2, (windowHeight- textTex->GetHeight()) / 2));
	}

	void Render(SDL_Renderer* renderer) override
	{
		RenderType<Text>(renderer);
	}

	void Update(double deltaTime) override
	{
		
	}

	void HandleEvent(SDL_Event& e) override
	{
		if (e.type == SDL_KEYDOWN) {
			Engine::GetInstance()->LoadScene("SampleScene", "SampleScene");
			Engine::GetInstance()->RemoveScene("GameOverScene");
		}
	}
};