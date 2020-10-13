#include <cassert>
#include "SceneController.h"
#include "PlayScene.h"

using namespace std;

SceneController::SceneController()
{
	ChangeScene(make_shared<PlayScene>(*this));
}


SceneController::~SceneController()
{
}

void SceneController::SceneUpdate()
{
	// �X�^�b�N�̈�ԏ�̃V�[�����X�V
	_scene.front()->Update();

	// �X�^�b�N�̈�ԉ����珇�Ԃɕ`��
	for (auto rit = _scene.rbegin(); rit != _scene.rend(); rit++)
	{
		(*rit)->Draw();
	}
}

void SceneController::ChangeScene(shared_ptr<Scene> scene)
{
	if (_scene.size() > 0)
	{
		_scene.front()->Off();
	}
	_scene.clear();
	_scene.push_front(scene);
	scene->On();
}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{
	if (_scene.size() > 0)
	{
		_scene.front()->Off();
	}
	_scene.push_front(scene);
	scene->On();
}

void SceneController::PopScene(void)
{
	assert(_scene.size() > 0);
	_scene.front()->Off();
	_scene.pop_front();
	if (_scene.size() > 0)
	{
		_scene.front()->On();
	}
}

Scene& SceneController::GetCurrentScene()
{
	assert(_scene.size() > 0);
	return *_scene.front();
}