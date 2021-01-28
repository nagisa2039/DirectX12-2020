#pragma once
#include "../Scene/SceneController.h"

class Input;

/// <summary>
/// �V�[���̊��N���X
/// </summary>
class Scene
{
public:
	/// <param name="ctrl">�V�[���Ǘ��N���X</param>
	Scene(SceneController & controller);
	virtual ~Scene();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// �V�[���L�����C�x���g
	/// </summary>
	virtual void On() {};

	/// <summary>
	/// �V�[���������C�x���g
	/// </summary>
	virtual void Off() {};

protected:
	SceneController& controller_;
};

