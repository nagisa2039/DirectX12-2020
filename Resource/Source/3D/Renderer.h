#pragma once
#include <vector>
#include <memory>

class Mesh;
class SceneInf;

/// <summary>
/// 3D描画クラスのインターフェース
/// </summary>
class Renderer
{
public:
	/// <param name="sceneInf">シーン情報管理クラス</param>
	Renderer(std::shared_ptr<SceneInf>& sceneInf);

	/// <summary>
	/// Computeによる更新
	/// Rendererから呼ぶためにUpdateから分離
	/// </summary>
	virtual void ComputeUpdate(std::vector<Mesh*>& meshs);

	/// <summary>
	/// 通常描画
	/// </summary>
	/// <param name="meshs">描画するメッシュ</param>
	virtual void Draw(std::vector<Mesh*>& meshs) = 0;

	/// <summary>
	/// 影描画
	/// </summary>
	/// <param name="meshs">描画するメッシュ</param>
	virtual void DrawShadow(std::vector<Mesh*>& meshs)=0;

protected:
	std::shared_ptr<SceneInf>& sceneInf_;
};

