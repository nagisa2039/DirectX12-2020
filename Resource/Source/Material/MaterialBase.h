#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;
using int4	 = DirectX::XMINT4;
using matrix = DirectX::XMMATRIX;
#endif

// マテリアル基本情報
struct MaterialBase
{
	float4 diffuse;		//拡散反射
	float3 specular;	//鏡面反射
	float3 ambient;		//環境光成分
	float power;		// スペキュラ乗数
	int textureIndex;	// テクスチャインデックス
};