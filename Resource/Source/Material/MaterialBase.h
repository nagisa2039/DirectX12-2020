#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;
using int4	 = DirectX::XMINT4;
using matrix = DirectX::XMMATRIX;
#endif

struct MaterialBase
{
	float4 diffuse;//�g�U����
	float3 specular;//���ʔ���
	float3 ambient;//��������
	float power;	// �X�y�L�����搔
	int textureIndex;
};