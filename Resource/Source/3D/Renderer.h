#pragma once
#include <vector>
#include <memory>

class Mesh;
class Camera;

/// <summary>
/// 3D描画クラスのインターフェース
/// </summary>
class Renderer
{
public:
	/// <param name="camera">カメラクラス</param>
	Renderer(std::shared_ptr<Camera>& camera);

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
	std::shared_ptr<Camera>& camera_;
};

