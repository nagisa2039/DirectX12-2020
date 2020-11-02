#include "Player.h"
#include "Animator.h"
#include "Utility/Input.h"
#include "System/Application.h"

using namespace std;

Player::Player()
{
	animator_ = make_unique<Animator>();
	animator_->FolderImageReadAndInitAnim(L"idle", 6, true, L"Resource/Image/Hero Knight/Sprites/HeroKnight/Idle/HeroKnight_Idle_");
	animator_->ChengeAnim(L"idle");

	pos_ = Vector2f();
}

Player::~Player()
{
}

void Player::Update()
{
	animator_->Update();

	auto& input = Application::Instance().GetInput();
	auto move = [input = input](const unsigned char keycode, float& target, const float speed) 
	{
		if (input.GetButton(keycode))
		{
			target += speed;
		}
	};

	const float speed = 2.0f;
	move(DIK_UP, pos_.y, -speed);
	move(DIK_DOWN, pos_.y, +speed);
	move(DIK_LEFT, pos_.x, -speed);
	move(DIK_RIGHT, pos_.x, +speed);
}

void Player::Draw()
{
	animator_->Draw(pos_.ToVector2<int>(), 1.0f, false);
}
