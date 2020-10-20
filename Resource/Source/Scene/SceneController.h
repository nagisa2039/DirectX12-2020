#pragma once
#include <stack>
#include <memory>

class Scene;

class SceneController
{
public:
	SceneController();
	~SceneController();

	// シーンの更新
	void SceneUpdate();

	// シーンを変更
	// @param scene	遷移するシーンのunique_ptr
	void ChangeScene(std::shared_ptr<Scene> scene);

	// シーンのスタック
	// @param scene	スタックするシーンのunique_ptr
	void PushScene(std::shared_ptr<Scene> scene);

	// シーンのポップ
	void PopScene(void);

	/// <summary>
	/// 現在のシーン参照の取得
	/// </summary>
	Scene& GetCurrentScene();

private:
	std::deque<std::shared_ptr<Scene>> scene_;
};

