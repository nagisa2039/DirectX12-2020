#pragma once
#include "../Scene/SceneController.h"

class Input;
// シーンの基底クラス
class Scene
{
public:
	Scene(SceneController & controller);
	virtual ~Scene();
	// シーンの更新を行う
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void On() {};
	virtual void Off() {};

protected:
	SceneController& _controller;
};

