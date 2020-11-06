#pragma once

/// <summary>
/// 3D描画クラスのインターフェース
/// </summary>
class Renderer
{
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawShadow()=0;
};

