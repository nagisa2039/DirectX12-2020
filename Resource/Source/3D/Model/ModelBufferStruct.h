#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float4 = DirectX::XMFLOAT4;
using uint = uint32_t;
using min16uint = uint16_t;
#endif

struct MaterialStruct
{
	float4 diffuse;//拡散反射
	float4 specular;//鏡面反射
	float4 ambient;//環境光成分
	float power;	// スペキュラ乗数

	// テクスチャ
	int texIdx;
	// スフィアマップ	乗算
	int sphIdx;
	// スフィアマップ	加算
	int spaIdx;
	// 追加テクスチャ
	int addtexIdx;
	// toonテクスチャ
	int toonIdx;
};

struct MaterialIndex
{
	uint index;
};