#pragma once
#include <vector>
#include <memory>

class Mesh;

/// <summary>
/// 3D描画クラスのインターフェース
/// </summary>
class Renderer
{
public:
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
};

