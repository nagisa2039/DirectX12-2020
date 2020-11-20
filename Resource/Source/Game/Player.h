#pragma once
#include <memory>
#include <DirectXMath.h>

class Animator;

class Player
{
public:
	Player();
	~Player();

	void Update();
	void Draw();

private:
	DirectX::XMFLOAT2 pos_;
	std::unique_ptr<Animator> animator_;
};

