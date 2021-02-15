#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using matrix = DirectX::XMMATRIX;
using float4 = DirectX::XMFLOAT4;
using float3 = DirectX::XMFLOAT3;
#endif

// GPUに送る定数データ

// 基本情報
struct Utility
{
	float time;			// 時間
};

// シーン情報
struct SceneStruct
{
	// カメラのビュー行列
	matrix view;
	// カメラのプロジェクション行列
	matrix proj;
	// カメラプロジェクションの逆行列
	matrix invProj;
	// 影用行列
	matrix shadow;
	// カメラの回転行列
	matrix cameraRotate;
	// ライトの行列
	matrix lightCamera;
	// ライトの向き
	float3 lightVec;
	float dummy0;

	// 視点
	float3 eye;
	// 視野角
	float fov;
};