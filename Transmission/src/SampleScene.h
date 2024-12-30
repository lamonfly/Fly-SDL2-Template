// Base needs
#include "Scene/Scene.h"

// Components
#include <Physics/Transform.h>
#include <Physics/Collider/Collider.h>
#include <Physics/Collider/Shapes/RectShape.h>
#include <Physics/Collider/Shapes/CircleShape.h>
#include <Graphics/Sprite.h>
#include <Graphics/Circle.h>
#include <Graphics/Text.h>
#include "Breakout/PlayerPlatform.h"
#include "Breakout/BallMovement.h"
#include "Breakout/Brick.h"

class SampleScene : public Scene 
{
private:
	Texture* brickBrokenTex = new Texture();
	int32_t topWallID;
public:

	void Init() override  
	{
		int windowHeight = Engine::GetInstance()->GetWindow()->GetResolutionHeight();
		int windowWidth = Engine::GetInstance()->GetWindow()->GetResolutionWidth();

		//Bricks values
		float brickTotalHeight = windowHeight / 3;
		int brickCountHeight = 8;
		int brickCountWidth = 15;
		float brickSpacing = 1.5f;
		int textureMul = 3;

		//Calculate
		float brickHeight = (float)(brickTotalHeight - brickSpacing * brickCountHeight) / brickCountHeight;
		float brickWidth = (float)(windowWidth - brickSpacing * (brickCountWidth + 1)) / brickCountWidth;

		//Texture
		auto brickTex = new Texture(); 
		brickTex->LoadFromFile("res/ice.png");
		brickTex->SetAlpha(127.5);
		brickTex->SetSize(brickWidth, brickHeight);

		brickBrokenTex->LoadFromFile("res/ice_cracked.png");
		brickBrokenTex->SetAlpha(127.5);
		brickBrokenTex->SetSize(brickWidth, brickHeight);

		auto shape = new RectShape(Vector2(brickWidth, brickHeight));

		for (float h = windowHeight - brickHeight - brickSpacing; h >= (windowHeight - brickTotalHeight); h -= (brickHeight + brickSpacing)) {
			for (float w = brickSpacing; w <= windowWidth; w += (brickWidth + brickSpacing)) {
				auto brick = mRegistry.create();
				mRegistry.emplace<Transform>(brick, Vector2(w,h));
				int wTex = rand() % static_cast<int>(1024 - brickWidth * textureMul);
				int hTex = rand() % static_cast<int>(1024 - brickHeight * textureMul);
				SDL_Rect* clip = new SDL_Rect{ wTex, hTex, wTex + static_cast<int>(brickWidth * textureMul), hTex + static_cast<int>(brickHeight * textureMul) };
				mRegistry.emplace<Sprite>(brick, brickTex).SetClip(clip);
				mRegistry.emplace<Collider>(brick, shape);
				mRegistry.emplace<Brick>(brick);
			}
		}

		//Plaform values
		int platformWidth = brickWidth * 2;

		auto platform = mRegistry.create();
		mRegistry.emplace<Transform>(platform, Vector2((float)(windowWidth - platformWidth) / 2, brickSpacing));
		mRegistry.emplace<Collider>(platform, new RectShape(Vector2(platformWidth, brickHeight)));
		mRegistry.emplace<PlayerPlatform>(platform, windowWidth - platformWidth);

		//Texture
		auto platformTex = new Texture();
		platformTex->LoadFromFile("res/wood.jpg");
		platformTex->SetSize(platformWidth, brickHeight);
		mRegistry.emplace<Sprite>(platform, platformTex).SetClip(new SDL_Rect{0, 0, platformWidth * 4, static_cast<int>(brickHeight) * 4 });

		//Ball values
		auto ball = mRegistry.create();
		mRegistry.emplace<Transform>(ball, Vector2(windowWidth/ 2, brickHeight + brickSpacing + 10));
		mRegistry.emplace<Collider>(ball, new CircleShape(6));
		mRegistry.emplace<BallMovement>(ball);

		//Texture
		auto ballTex = new Texture();
		ballTex->LoadFromFile("res/swirlstroke_red.png");
		ballTex->SetSize(12, 12);
		mRegistry.emplace<Sprite>(ball, ballTex);

		//Walls
		auto leftWall = mRegistry.create();
		mRegistry.emplace<Transform>(leftWall, Vector2(-10, 0));
		mRegistry.emplace<Collider>(leftWall, new RectShape(Vector2(10, windowHeight)));
		auto rightWall = mRegistry.create();
		mRegistry.emplace<Transform>(rightWall, Vector2(windowWidth + 1, 0));
		mRegistry.emplace<Collider>(rightWall, new RectShape(Vector2(10, windowHeight)));
		auto bottomWall = mRegistry.create();
		mRegistry.emplace<Transform>(bottomWall, Vector2(-10, windowHeight));
		mRegistry.emplace<Collider>(bottomWall, new RectShape(Vector2(windowWidth + 20, 10)));
		auto topWall = mRegistry.create();
		mRegistry.emplace<Transform>(topWall, Vector2(-10, -25));
		mRegistry.emplace<Collider>(topWall, new RectShape(Vector2(windowWidth + 50, 10)));
		topWallID = entt::to_integral(topWall);

		entt::entity sampleText = mRegistry.create();
		auto textTex = new Texture();
		textTex->LoadText(TTF_OpenFont("res/Roboto-Regular.ttf", 24), "Press anything to play!", { 0, 0, 0 });
		mRegistry.emplace<Text>(sampleText, textTex);
		mRegistry.emplace<Transform>(sampleText, Vector2((windowWidth - textTex->GetWidth()) / 2, (windowHeight - textTex->GetHeight()) / 2));
	}

	void Render(SDL_Renderer* renderer) override
	{
		RenderType<Sprite>(renderer);
		RenderType<Text>(renderer);
	}

	void Update(double deltaTime) override
	{
		// Update platform
		for (auto&& [entity, transform, playerPlatform] : mRegistry.view<Transform, PlayerPlatform>().each()) {
			transform.Position.X += deltaTime * playerPlatform.GetDirection() * playerPlatform.GetSpeed();
			transform.Position.X = std::fmax(0, std::fmin(transform.Position.X, playerPlatform.GetMaxPosition()));
		}

		// Update ball
		for (auto&& [entity, transform, ballMovement, collider] : mRegistry.view<Transform, BallMovement, Collider>().each()) {
			ballMovement.UpdateMove(deltaTime, transform);

			// Rotate ball
			transform.setRotation(transform.getRotation() + 100.f * deltaTime);

			for (auto collision : collider.GetCollisions()) {
				if (collider.HasEntered(collision.first)) {
					ballMovement.Reflect(collision.second->Normal);
				}

				// End game
				if (collision.first == topWallID) {
					Engine::GetInstance()->RemoveScene("SampleScene");
					Engine::GetInstance()->LoadScene("GameOverScene", "GameOverScene");
				}
			}
		}

		// Update bricks
		for (auto&& [entity, transform, brick, collider] : mRegistry.view<Transform, Brick, Collider>().each()) {
			for (auto collision : collider.GetCollisions()) {
				if (collider.HasEntered(collision.first)) {
					brick.TakeDamage();
					if (brick.Health < 0) {
						// Remove entity
						mRegistry.remove<Transform>(entity);
						mRegistry.remove<Collider>(entity);
						mRegistry.remove<Brick>(entity);
					}
					else {
						// Replace texture
						mRegistry.get<Sprite>(entity).SetTexture(brickBrokenTex);
					}
				}
			}
		}
	}

	void HandleEvent(SDL_Event& e) override
	{
		for (auto&& [entity, transform, playerPlatform] : mRegistry.view<Transform, PlayerPlatform>().each()) {
			playerPlatform.HandleEvent(e, transform);
		}

		HandleEventType<BallMovement>(e);

		for (auto&& [entity, text] : mRegistry.view<Text>().each()) {
			if (e.type == SDL_KEYDOWN) {
				mRegistry.destroy(entity);
			}
		}
	}
};