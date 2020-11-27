#pragma once
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <DirectXMath.h>

class Command;

class Camera
{
public:
	Camera(Command& cmd, ID3D12Device& dev);
	~Camera();

	void Update();

	/// <summary>
	/// カメラのヒープを設定
	/// </summary>
	/// <param name="rootParamIdx">rootSignatureインデックス</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// カメラの座標取得
	/// </summary>
	DirectX::XMFLOAT3 GetCameraPosition()const;

	/// <summary>
	/// カメラのターゲット座標の取得
	/// </summary>
	DirectX::XMFLOAT3 GetCameraTarget()const;

	/// <summary>
	/// カメラの座標設定
	/// </summary>
	/// <param name="pos">カメラ座標</param>
	void SetCameraPosision(const DirectX::XMFLOAT3& pos);

	/// <summary>
	/// カメラのターゲット座標設定
	/// </summary>
	/// <param name="target">ターゲット座標</param>
	void SetCameraTarget(const DirectX::XMFLOAT3& target);

	/// <summary>
	/// カメラ行列の更新
	/// </summary>
	void UpdateCamera();

private:
	Command& cmd_;
	ID3D12Device& dev_;

	// カメラ行列用定数バッファ
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;

	// GPUに渡す行列をまとめた構造体
	struct Scene
	{
		//DirectX::XMMATRIX world;	// 合成前ワールド
		DirectX::XMMATRIX view;	//
		DirectX::XMMATRIX proj;	//
		DirectX::XMMATRIX invProj;	// プロジェクションの逆行列
		DirectX::XMMATRIX lightCamera;	//ライドから見たビュープロジェクション
		DirectX::XMMATRIX shadow;
		DirectX::XMFLOAT3 eye;	// 視点
	};

	// 定数バッファのアドレスを格納	
	// _cameraCBの内容を変更したいときはこいつを通じて変更してね
	Scene* mappedCam_;

	DirectX::XMFLOAT3 eye_;
	DirectX::XMFLOAT3 target_;
	DirectX::XMFLOAT3 up_;
	float fov_;

	// カメラの作成
	bool CreateCameraConstantBufferAndView();
};

