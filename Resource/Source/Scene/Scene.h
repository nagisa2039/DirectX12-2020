#pragma once
#include "../Scene/SceneController.h"

class Input;

/// <summary>
/// シーンの基底クラス
/// </summary>
class Scene
{
public:
	/// <param name="ctrl">シーン管理クラス</param>
	Scene(SceneController & controller);
	virtual ~Scene();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// シーン有効時イベント
	/// </summary>
	virtual void On() {};

	/// <summary>
	/// シーン無効時イベント
	/// </summary>
	virtual void Off() {};

protected:
	SceneController& controller_;
};

