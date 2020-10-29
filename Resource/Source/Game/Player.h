#pragma once
#include <memory>
#include "Utility/Geometry.h"

class Animator;

class Player
{
public:
	Player();
	~Player();

	void Update();
	void Draw();

private:
	Vector2f pos_;
	std::unique_ptr<Animator> animator_;
};

