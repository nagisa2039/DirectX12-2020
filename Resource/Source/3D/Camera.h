#pragma once
#include "3D/Component.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "3D/Actor.h"
#include <functional>
#include <array>

/// <summary>
/// カメラクラス
/// </summary>
class Camera
	: public Component
{
public:
	/// <param name="owner">所有者</param>
	Camera(std::weak_ptr<Actor>owner);
	~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// ターゲット座標の取得
	/// </summary>
	DirectX::XMFLOAT3 GetTargetPos()const;

	/// <summary>
	/// View行列の取得
	/// </summary>
	DirectX::XMMATRIX GetViewMatrix()const;

	/// <summary>
	/// Projection行列の取得
	/// </summary>
	DirectX::XMMATRIX GetProjMatrix()const;

	/// <summary>
	/// FOVの取得
	/// </summary>
	float GetFOV()const;

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void DrawImGui()override;

	// Projection行列の設定
	enum class ProjectionMode
	{
		// 透視投影
		Perspective,
		// 正投影
		Orthographic,
		Max
	};

	/// <summary>
	/// 投影モードを設定
	/// </summary>
	/// <param name="pm">投影モード</param>
	void SetProjectionMode(const ProjectionMode pm);

private:
	using ProjectionFunc = std::function<DirectX::XMMATRIX(float, float, float)>;
	std::array<ProjectionFunc, static_cast<size_t>(ProjectionMode::Max)> projectionFuncTable_;
	ProjectionMode projectionMode_;

	float fov_;
};

/// <summary>
/// カメラオブジェクト
/// </summary>
class CameraObject
	: public Actor
{
public:
	/// <param name="keybordMove">キーボードでの移動を有効化するか</param>
	CameraObject(const bool keybordMove);
	~CameraObject();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="pm">投影モード</param>
	void Init(const Camera::ProjectionMode pm);

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// カメラの取得
	/// </summary>
	std::shared_ptr<Camera>& GetCamera();

private:
	std::shared_ptr<Camera> camera_;
	bool keybordMove_;
};