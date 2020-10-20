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
	// スタックの一番上のシーンを更新
	scene_.front()->Update();

	// スタックの一番下から順番に描画
	for (auto rit = scene_.rbegin(); rit != scene_.rend(); rit++)
	{
		(*rit)->Draw();
	}
}

void SceneController::ChangeScene(shared_ptr<Scene> scene)
{
	if (scene_.size() > 0)
	{
		scene_.front()->Off();
	}
	scene_.clear();
	scene_.push_front(scene);
	scene->On();
}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{
	if (scene_.size() > 0)
	{
		scene_.front()->Off();
	}
	scene_.push_front(scene);
	scene->On();
}

void SceneController::PopScene(void)
{
	assert(scene_.size() > 0);
	scene_.front()->Off();
	scene_.pop_front();
	if (scene_.size() > 0)
	{
		scene_.front()->On();
	}
}

Scene& SceneController::GetCurrentScene()
{
	assert(scene_.size() > 0);
	return *scene_.front();
}