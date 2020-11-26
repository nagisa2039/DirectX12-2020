#pragma once
#include <memory>

class Actor;

/// <summary>
/// �A�N�^�[�ɏ��L������N���X�̊��N���X
/// </summary>
class Component
	: public std::enable_shared_from_this<Component>
{
public:
	/// <summary>
	/// ���L�҂̃A�N�^�[
	/// </summary>
	/// <param name="owner">�A�N�^�[</param>
	Component(std::shared_ptr<Actor>owner);

	/// <summary>
	/// ���L�҂�Actor�擾
	/// </summary>
	std::weak_ptr<Actor> GetOwner();

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update() = 0;

private:
	std::weak_ptr<Actor> owner_;
};