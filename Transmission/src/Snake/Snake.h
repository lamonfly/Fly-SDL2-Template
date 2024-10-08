#pragma once

#include <SDL.h>
#include <deque>
#include <vector>
#include <random>
#include <Event/Eventable.h>
#include <Graphics/Renderable.h>
#include <Physics/Updateable.h>
#include "SnakePoint.h"

class Snake : Renderable, Eventable, Updateable
{
private:
	std::deque<SnakePoint> positions = {};
	Transform* transform;
	Vector2 nextTurn;
	double speed = 10;
	int lastKeyPress = 0;
	int currentKeyPress = 0;
	int length = 3;

	// Get rectangle rect
	SDL_FRect getRectForVectors(Vector2 a, Vector2 b);
	// Get step ceiling
	Vector2 getStepCeil(Vector2 a);
	// Get step floor
	Vector2 getStepFloor(Vector2 a);
	// Check if input is opposite
	bool isInputOpposite(int a, int b);
	// Get all position
	std::vector<SnakePoint> getAllPositions();
	// Check point on line
	bool pointOnSnake(Vector2 point);
	// Check line on line
	bool headOnSnake();
public:
	SDL_Color Color = { 255,255,255,255 };

	// Initializes variables
	Snake() = default;

	Snake(Transform* snakeTransform);

	// Deallocates memorys
	~Snake() = default;

	// Draw snake
	void Render(SDL_Renderer* renderer, Transform transform) override;

	// Move snake
	void Update(double deltaTime) override;

	// Record input
	void HandleEvent(SDL_Event& e, Transform& transform) override;

	// Add length
	void EatApple(Transform* appleTransform);
};

