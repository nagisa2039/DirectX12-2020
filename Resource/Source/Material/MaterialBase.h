#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float3 = DirectX::XMFLOAT3;
#endif

struct MaterialBase
{
	float3 diffuse;//ŠgU”½Ë
	float3 specular;//‹¾–Ê”½Ë
	float3 ambient;//ŠÂ‹«Œõ¬•ª
	float power;	// ƒXƒyƒLƒ…ƒ‰æ”
	int textureIndex;
};