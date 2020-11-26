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
	virtual void Draw(std::vector<std::shared_ptr<Mesh>>& models) = 0;
	virtual void DrawShadow(std::vector<std::shared_ptr<Mesh>>& models)=0;
};

