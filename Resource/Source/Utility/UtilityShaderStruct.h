#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using matrix = DirectX::XMMATRIX;
using float4 = DirectX::XMFLOAT4;
using float3 = DirectX::XMFLOAT3;
#endif

struct Utility
{
	float time;			// ����
	float emmisionRate;	// �P�x�o�͉��Z���̏搔
};

struct SceneStruct
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix shadow;
	matrix lightCamera;
	float3 lightVec;
	float gomi0;
	float3 eye; // ���_
	float gomi1;
};