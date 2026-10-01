// Base needs
#include "Scene/Scene.h"

// Components
#include <Physics/Transform.h>
#include <Physics/RigidBody.h>
#include <Physics/PhysicsConfig.h>
#include <Graphics/Sprite.h>
#include <Graphics/Circle.h>
#include <Graphics/Text.h>
#include "Breakout/BreakoutLayers.h"
#include "Breakout/PlayerPlatform.h"
#include "Breakout/BallMovement.h"
#include "Breakout/Brick.h"

class SampleScene : public Scene
{
private:
	Texture* brickBrokenTex = new Texture();
	entt::entity topWall = entt::null;
public:

	void Init() override
	{
		int windowHeight = Engine::GetInstance()->GetWindow()->GetResolutionHeight();
		int windowWidth = Engine::GetInstance()->GetWindow()->GetResolutionWidth();

		//Physics
		PhysicsConfig physicsConfig;
		physicsConfig.Gravity = Vector2(0.0f, 0.0f);
		SetPhysicsConfig(physicsConfig);

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

		for (float h = windowHeight - brickHeight - brickSpacing; h >= (windowHeight - brickTotalHeight); h -= (brickHeight + brickSpacing)) {
			for (float w = brickSpacing; w <= windowWidth; w += (brickWidth + brickSpacing)) {
				auto brick = mRegistry.create();
				mRegistry.emplace<Transform>(brick, Vector2(w,h));
				int wTex = rand() % static_cast<int>(1024 - brickWidth * textureMul);
				int hTex = rand() % static_cast<int>(1024 - brickHeight * textureMul);
				SDL_Rect* clip = new SDL_Rect{ wTex, hTex, wTex + static_cast<int>(brickWidth * textureMul), hTex + static_cast<int>(brickHeight * textureMul) };
				mRegistry.emplace<Sprite>(brick, brickTex).SetClip(clip);
				auto brickBody = RigidBody::Box(Vector2(brickWidth, brickHeight), BodyType::Static);
				brickBody.Layer = BreakoutLayer::Brick;
				brickBody.CollidesWith = BreakoutLayer::Ball;
				mRegistry.emplace<RigidBody>(brick, brickBody);
				mRegistry.emplace<Brick>(brick);
			}
		}

		//Plaform values
		int platformWidth = brickWidth * 2;

		auto platform = mRegistry.create();
		mRegistry.emplace<Transform>(platform, Vector2((float)(windowWidth - platformWidth) / 2, brickSpacing));
		auto platformBody = RigidBody::Box(Vector2(platformWidth, brickHeight), BodyType::Kinematic);
		platformBody.Layer = BreakoutLayer::Paddle;
		platformBody.CollidesWith = BreakoutLayer::Ball;
		mRegistry.emplace<RigidBody>(platform, platformBody);
		mRegistry.emplace<PlayerPlatform>(platform, windowWidth - platformWidth);

		//Texture
		auto platformTex = new Texture();
		platformTex->LoadFromFile("res/wood.jpg");
		platformTex->SetSize(platformWidth, brickHeight);
		mRegistry.emplace<Sprite>(platform, platformTex).SetClip(new SDL_Rect{0, 0, platformWidth * 4, static_cast<int>(brickHeight) * 4 });

		//Ball values
		auto ball = mRegistry.create();
		mRegistry.emplace<Transform>(ball, Vector2(windowWidth/ 2, brickHeight + brickSpacing + 10));
		auto ballBody = RigidBody::Circle(6, BodyType::Dynamic);
		ballBody.Layer = BreakoutLayer::Ball;
		ballBody.CollidesWith = BreakoutLayer::Brick | BreakoutLayer::Wall | BreakoutLayer::Paddle;
		ballBody.Restitution = 1.0f;
		ballBody.Friction = 0.0f;
		ballBody.AllowSleeping = false;
		ballBody.UseCCD = true;
		mRegistry.emplace<RigidBody>(ball, ballBody);
		mRegistry.emplace<BallMovement>(ball);

		//Texture
		auto ballTex = new Texture();
		ballTex->LoadFromFile("res/swirlstroke_red.png");
		ballTex->SetSize(12, 12);
		mRegistry.emplace<Sprite>(ball, ballTex);

		//Walls
		auto makeWall = [&](Vector2 position, Vector2 size) {
			auto wall = mRegistry.create();
			mRegistry.emplace<Transform>(wall, position);
			auto wallBody = RigidBody::Box(size, BodyType::Static);
			wallBody.Layer = BreakoutLayer::Wall;
			wallBody.CollidesWith = BreakoutLayer::Ball;
			mRegistry.emplace<RigidBody>(wall, wallBody);
			return wall;
		};
		makeWall(Vector2(-10, 0), Vector2(10, windowHeight));                        // left
		makeWall(Vector2(windowWidth + 1, 0), Vector2(10, windowHeight));            // right
		makeWall(Vector2(-10, windowHeight), Vector2(windowWidth + 20, 10));         // bottom
		topWall = makeWall(Vector2(-10, -25), Vector2(windowWidth + 50, 10));        // top

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
		for (auto&& [entity, ballMovement, body] : mRegistry.view<BallMovement, RigidBody>().each()) {
			for (const Contact& contact : GetContacts(entity)) {
				if (contact.Phase != ContactPhase::Enter) continue;

				// End game
				if (contact.Other == topWall) {
					Engine::GetInstance()->RemoveScene("SampleScene");
					Engine::GetInstance()->LoadScene("GameOverScene", "GameOverScene");
					break;
				}

				// Speed up
				ballMovement.OnBounce();
				Vector2 direction = GetPhysics().GetLinearVelocity(body).Normalized();
				GetPhysics().SetLinearVelocity(body, direction * ballMovement.Speed);
			}
		}

		// Update bricks
		for (auto&& [entity, transform, brick, body] : mRegistry.view<Transform, Brick, RigidBody>().each()) {
			for (const Contact& contact : GetContacts(entity)) {
				if (contact.Phase != ContactPhase::Enter) continue;

				brick.TakeDamage();
				if (brick.Health < 0) {
					// Remove entity
					mRegistry.remove<Transform>(entity);
					mRegistry.remove<RigidBody>(entity);
					mRegistry.remove<Brick>(entity);
					break;
				}
				else {
					// Replace texture
					mRegistry.get<Sprite>(entity).SetTexture(brickBrokenTex);
				}
			}
		}
	}

	void HandleEvent(SDL_Event& e) override
	{
		for (auto&& [entity, transform, playerPlatform] : mRegistry.view<Transform, PlayerPlatform>().each()) {
			playerPlatform.HandleEvent(e, transform);
		}

		// Launch ball
		if (e.type == SDL_KEYDOWN) {
			for (auto&& [entity, ballMovement, body] : mRegistry.view<BallMovement, RigidBody>().each()) {
				if (ballMovement.Launched) continue;
				ballMovement.Launched = true;
				Vector2 direction = Vector2(rand() % 201 + (-100), rand() % 101).Normalized();
				GetPhysics().SetLinearVelocity(body, direction * ballMovement.Speed);
				GetPhysics().SetAngularVelocity(body, ballMovement.SpinDegPerSec);
			}
		}

		for (auto&& [entity, text] : mRegistry.view<Text>().each()) {
			if (e.type == SDL_KEYDOWN) {
				mRegistry.destroy(entity);
			}
		}
	}
};
