#pragma once
#include "Utility/ComPtr.h"
#include "3D/Component.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "3D/Actor.h"

struct SceneStruct;
class Command;

/// <summary>
/// カメラクラス
/// </summary>
class Camera
	: public Component
{
public:
	Camera(Command& cmd, ID3D12Device& dev, std::weak_ptr<Actor>owner);
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
	/// カメラのヒープを設定
	/// </summary>
	/// <param name="rootParamIdx">rootSignatureインデックス</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// ターゲット座標の取得
	/// </summary>
	DirectX::XMFLOAT3 GetTargetPos()const;

	/// <summary>
	/// ターゲット座標の設定
	/// </summary>
	/// <param name="tpos">ターゲット座標</param>
	void SetTargetPos(const DirectX::XMFLOAT3& tpos);

	/// <summary>
	/// ターゲットへのベクトルを取得する
	/// </summary>
	DirectX::XMFLOAT3 GetTargetVec()const;

	/// <summary>
	/// View行列の取得
	/// </summary>
	DirectX::XMMATRIX GetViewMatrix()const;

	/// <summary>
	/// Projection行列の取得
	/// </summary>
	DirectX::XMMATRIX GetProjMatrix()const;

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void DrawImGui()override;

private:
	Command& cmd_;
	ID3D12Device& dev_;

	// カメラ行列用定数バッファ
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;

	// 定数バッファのアドレスを格納	
	// _cameraCBの内容を変更したいときはこいつを通じて変更してね
	SceneStruct* mappedScene_;

	DirectX::XMFLOAT3 target_;
	float fov_;

	// カメラの作成
	bool CreateCameraConstantBufferAndView();

	/// <summary>
	/// カメラ行列の更新
	/// </summary>
	void UpdateCamera();
};

/// <summary>
/// カメラオブジェクト
/// </summary>
class CameraObject
	: public Actor
{
public:
	/// <param name="cmd"></param>
	/// <param name="dev"></param>
	CameraObject(Command& cmd, ID3D12Device& dev);
	~CameraObject();

	void Update()override;

	std::shared_ptr<Camera>& GetCamera();

private:
	Command& cmd_;
	ID3D12Device& dev_;
	std::shared_ptr<Camera> camera_;
};