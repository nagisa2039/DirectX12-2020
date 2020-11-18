#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float3 = DirectX::XMFLOAT3;
#endif

struct MaterialBase
{
	float3 diffuse;//�g�U����
	float3 specular;//���ʔ���
	float3 ambient;//��������
	float power;	// �X�y�L�����搔
	int textureIndex;
};