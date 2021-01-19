#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float2 = DirectX::XMFLOAT2;
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;
using int2 = DirectX::XMINT2;
using int3 = DirectX::XMINT3;
using int4 = DirectX::XMINT4;
using uint = unsigned int;
using uint2 = DirectX::XMUINT2;
using uint3 = DirectX::XMUINT3;
using uint4 = DirectX::XMUINT4;
using matrix = DirectX::XMMATRIX;
#endif

struct SettingData
{
	uint outline;
	float3 outlineColor;

	uint emmision;
	float3 emmisionColor;

	float emmisionRate;
	uint dissolve;
};