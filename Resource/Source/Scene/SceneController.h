#pragma once
#include <stack>
#include <memory>

class Scene;

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneController
{
public:
	SceneController();
	~SceneController();

	/// <summary>
	/// シーンの更新
	/// </summary>
	void SceneUpdate();

	/// <summary>
	/// シーンを変更
	/// </summary>
	/// <param name="scene">遷移するシーンのunique_ptr</param>
	void ChangeScene(std::shared_ptr<Scene> scene);

	/// <summary>
	/// シーンのスタック
	/// </summary>
	/// <param name="scene">スタックするシーンのunique_ptr</param>
	void PushScene(std::shared_ptr<Scene> scene);

	/// <summary>
	/// シーンのポップ
	/// 現在再生中のシーンを削除し1つ戻る
	/// </summary>
	void PopScene(void);

	/// <summary>
	/// 現在のシーン参照の取得
	/// </summary>
	Scene& GetCurrentScene();

private:
	std::deque<std::shared_ptr<Scene>> scene_;
};

