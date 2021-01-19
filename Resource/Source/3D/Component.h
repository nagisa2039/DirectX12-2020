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
	Component(std::weak_ptr<Actor>owner);

	virtual ~Component()= default;

	/// <summary>
	/// ���L�҂�Actor�擾
	/// </summary>
	std::weak_ptr<Actor> GetOwner()const;

	/// <summary>
	/// ������
	/// �R���X�g���N�^�łł��Ȃ�(���g������Ȃ��Əo���Ȃ�)
	/// �������s��
	/// </summary>
	virtual void Init()=0;

	/// <summary>
	/// ���t���[���X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// ImGui�̕`��
	/// �p���K�{�ł͂Ȃ�
	/// </summary>
	virtual void DrawImGui();

private:
	std::weak_ptr<Actor> owner_;
};