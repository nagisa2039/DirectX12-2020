#pragma once
#include "../Scene/SceneController.h"

class Input;
// �V�[���̊��N���X
class Scene
{
public:
	Scene(SceneController & controller);
	virtual ~Scene();
	// �V�[���̍X�V���s��
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void On() {};
	virtual void Off() {};

protected:
	SceneController& _controller;
};

