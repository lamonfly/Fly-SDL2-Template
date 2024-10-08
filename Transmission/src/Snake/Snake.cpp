#include "Snake.h"
#include <vector>
#include <cmath>
#include <Physics/Transform.h>
#include <Core/Engine.h>

Snake::Snake(Transform* snakeTransform)
{
	transform = snakeTransform;
}

SDL_FRect Snake::getRectForVectors(Vector2 a, Vector2 b)
{
	// Determine width and height based on point arrangement
	float width = fmax(transform->Scale, abs(b.X - a.X));
	float height = fmax(transform->Scale, abs(b.Y - a.Y));

	if (a.Magnitude() > b.Magnitude()) {
		return SDL_FRect(b.X, b.Y, width, height);
	}
	else {
		return SDL_FRect(a.X, a.Y, width, height);
	}
}

Vector2 Snake::getStepCeil(Vector2 a)
{
	return Vector2(ceil(a.X / transform->Scale) * transform->Scale, ceil(a.Y / transform->Scale) * transform->Scale);
}

Vector2 Snake::getStepFloor(Vector2 a)
{
	return Vector2(floor(a.X / transform->Scale) * transform->Scale, floor(a.Y / transform->Scale) * transform->Scale);
}

bool Snake::isInputOpposite(int a, int b)
{
	switch (a)
	{
	case SDLK_UP:
		return SDLK_DOWN == b;
	case SDLK_DOWN:
		return SDLK_UP == b;
	case SDLK_LEFT:
		return SDLK_RIGHT == b;
	case SDLK_RIGHT:
		return SDLK_LEFT == b;
	default:
		return true;
	}
}

std::vector<SnakePoint> Snake::getAllPositions()
{
	std::vector<SnakePoint> allPositions(positions.begin(), positions.end());
	allPositions.push_back(transform->Position);
	return allPositions;
}

bool Snake::pointOnSnake(Vector2 point)
{
	auto allPositions = getAllPositions();
	for (size_t i = 0; i < positions.size() - 1; i++) {
		if (!positions[i].Ignore) 
		{
			if (std::fabs(positions[i].Position.X - point.X) < 1e-6f && std::fabs(positions[i].Position.X - positions[i+1].Position.X) < 1e-6f) {
				float low = std::min(positions[i].Position.Y, positions[i + 1].Position.Y);
				float high = std::max(positions[i].Position.Y, positions[i + 1].Position.Y);
				if (point.Y > low && point.Y < high) {
					return true;
				}
			}
			else if (std::fabs(positions[i].Position.Y - point.Y) < 1e-6f && std::fabs(positions[i].Position.Y - positions[i+1].Position.Y) < 1e-6f) {
				float low = std::min(positions[i].Position.X, positions[i + 1].Position.X);
				float high = std::max(positions[i].Position.X, positions[i + 1].Position.X);
				if (point.X > low && point.X < high) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Snake::headOnSnake()
{
	Vector2 head = transform->Position;
	Vector2 next = positions.back().Position;

	if (positions.size() < 4) {
		return false;
	}

	for (size_t i = 0; i < positions.size() - 3; i++) {
		if (positions[i].Ignore) {
			// Find the four orientations needed for general and 
			// special cases 
			int o1 = head.Orientation(next, positions[i].Position);
			int o2 = head.Orientation(next, positions[i + 1].Position);
			int o3 = positions[i].Position.Orientation(positions[i + 1].Position, head);
			int o4 = positions[i].Position.Orientation(positions[i + 1].Position, next);

			// General case 
			if (o1 != o2 && o3 != o4)
				return true;

			// Special Cases 
			// p1, q1 and p2 are collinear and p2 lies on segment p1q1 
			if (o1 == 0 && head.OnSegment(positions[i].Position, next)) return true;

			// p1, q1 and q2 are collinear and q2 lies on segment p1q1 
			if (o2 == 0 && head.OnSegment(positions[i + 1].Position, next)) return true;

			// p2, q2 and p1 are collinear and p1 lies on segment p2q2 
			if (o3 == 0 && positions[i].Position.OnSegment(head, positions[i + 1].Position)) return true;

			// p2, q2 and q1 are collinear and q1 lies on segment p2q2 
			if (o4 == 0 && positions[i].Position.OnSegment(next, positions[i + 1].Position)) return true;

			return false;
		}
	}

	return false;
}



void Snake::Render(SDL_Renderer* renderer, Transform transform)
{
    // Set draw color
    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);

	// Draw snake
	std::vector<SnakePoint> allPositions = getAllPositions();

	if (allPositions.size() > 1) {
		for (int i = 0; i < allPositions.size() - 1; i++) {
			if (!allPositions[i].Ignore) {
				auto rect = getRectForVectors(allPositions[i + 1].Position, allPositions[i].Position);

				SDL_RenderFillRectF(renderer, &rect);
			}
		}
	}
	else {
		auto rect = SDL_FRect(allPositions[0].Position.X, allPositions[0].Position.Y, this->transform->Scale, this->transform->Scale);
		SDL_RenderFillRectF(renderer, &rect);
	}
}

void Snake::Update(double deltaTime)
{
	// Turn snake
	if (lastKeyPress != currentKeyPress &&
		(nextTurn.X <= transform->Position.X ||
		nextTurn.Y <= transform->Position.Y)) {
		SnakePoint nextPos = SnakePoint(getStepFloor(transform->Position));
		positions.push_back(nextPos);

		if (lastKeyPress == SDLK_RIGHT && currentKeyPress == SDLK_UP) {
			positions.back().Ignore = true;
			SnakePoint next = SnakePoint(Vector2(nextPos.Position.X, nextPos.Position.Y + transform->Scale));
			positions.push_back(next);
		}
		else if (lastKeyPress == SDLK_DOWN && currentKeyPress == SDLK_LEFT) {
			positions.back().Ignore = true;
			SnakePoint next = SnakePoint(Vector2(nextPos.Position.X + transform->Scale, nextPos.Position.Y + transform->Scale));
			positions.push_back(next);
		}

		transform->Position = nextPos.Position;
		lastKeyPress = currentKeyPress;
	}

	// Move snake
	switch (lastKeyPress)
	{
	case SDLK_UP:
		transform->Position.Y += -transform->Scale * speed * deltaTime;
		break;
	case SDLK_DOWN:
		transform->Position.Y += transform->Scale * speed * deltaTime;
		break;
	case SDLK_LEFT:
		transform->Position.X += -transform->Scale * speed * deltaTime;
		break;
	case SDLK_RIGHT:
		transform->Position.X += transform->Scale * speed * deltaTime;
		break;
	default:
		break;
	}

	// Mirror snake
	if (transform->Position.X > 720) {
		SnakePoint nextPos = SnakePoint(getStepFloor(transform->Position));
		nextPos.Ignore = true;
		positions.push_back(nextPos);
		transform->Position.X = 0;
		SnakePoint otherPos = SnakePoint(getStepCeil(transform->Position));
		positions.push_back(otherPos);

	}
	else if (transform->Position.X < 0) {
		SnakePoint nextPos = SnakePoint(getStepCeil(transform->Position));
		nextPos.Ignore = true;
		positions.push_back(nextPos);
		transform->Position.X = 720;
		SnakePoint otherPos = SnakePoint(getStepFloor(transform->Position));
		positions.push_back(otherPos);
	}
	else if (transform->Position.Y > 479.9) {
		SnakePoint nextPos = SnakePoint(getStepFloor(transform->Position));
		nextPos.Ignore = true;
		positions.push_back(nextPos);
		transform->Position.Y = 0;
		SnakePoint otherPos = SnakePoint(getStepCeil(transform->Position));
		positions.push_back(otherPos);
	}
	else if (transform->Position.Y < 0) {
		SnakePoint nextPos = SnakePoint(getStepCeil(transform->Position));
		nextPos.Ignore = true;
		positions.push_back(nextPos);
		transform->Position.Y = 479.9f;
		SnakePoint otherPos = SnakePoint(getStepCeil(transform->Position));
		positions.push_back(otherPos);
	}

	// Snake length
	double currentLength = 0;

	if (positions.size() > 0) {
		std::vector<SnakePoint> allPositions = getAllPositions();

		for (int i = 0; i < allPositions.size() - 1; i++) {
			if (!allPositions[i].Ignore) {
				currentLength += allPositions[i].Position.Distance(allPositions[i + 1].Position);
			}
		}

		double maxLength = transform->Scale * length;
		int index = 0;
		while (currentLength > maxLength) {
			double lastDistance = allPositions[index].Position.Distance(allPositions[index + 1].Position);
			if (allPositions[index].Ignore) {
				positions.pop_front();
				index++;
			}
			// Remove last
			else if (lastDistance <= (currentLength - maxLength)) {
				positions.pop_front();
				currentLength -= lastDistance;
				index++;
			}
			// Move last
			else {
				Vector2 direction = (allPositions[index + 1].Position - allPositions[index].Position).Normalized();
				Vector2 add = direction * (currentLength - maxLength);
				positions[0].Position += direction * (currentLength - maxLength);
				currentLength -= (currentLength - maxLength);
				break;
			}
		}
	}

	// Snake overlap
	if (positions.size() > 0) {
		if (headOnSnake()) {
			Engine::GetInstance()->RemoveScene("game");
			Engine::GetInstance()->LoadScene("game", "game");
		}
	}
}

void Snake::HandleEvent(SDL_Event& e, Transform& transform)
{
    // Record snake movement
	if (e.type == SDL_KEYDOWN && 
		e.key.keysym.sym != lastKeyPress &&
		!isInputOpposite(e.key.keysym.sym, lastKeyPress)) {
		currentKeyPress = e.key.keysym.sym;
		nextTurn = getStepCeil(this->transform->Position);
	}
}

void Snake::EatApple(Transform* appleTransform)
{
	length++;
	Vector2 newPos = getStepCeil(Vector2((rand() % 718) + 1, (rand() % 418) + 1));

	while (pointOnSnake(newPos)) {
		newPos = getStepCeil(Vector2((rand() % 718) + 1, (rand() % 418) + 1));
	}

	appleTransform->Position = newPos;
}
