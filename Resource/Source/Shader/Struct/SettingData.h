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

/// <summary>
/// �`��ݒ�
/// </summary>
struct SettingData
{
	// �A�E�g���C��
	uint outline;
	// �A�E�g���C���F
	float3 outlineColor;

	// �G�~�b�V����
	uint emmision;
	// �G�~�b�V�����F
	float3 emmisionColor;

	// �G�~�b�V�����̋���
	float emmisionRate;
	// �f�B�]���u
	uint dissolve;
	// �A���`�G�C���A�V���O
	uint antialiasing;
	// ��ʊE�[�x
	uint depth_of_field;
	// �C���X�^���X��
	uint instanceNum;
};