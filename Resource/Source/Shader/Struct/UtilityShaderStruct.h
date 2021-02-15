#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using matrix = DirectX::XMMATRIX;
using float4 = DirectX::XMFLOAT4;
using float3 = DirectX::XMFLOAT3;
#endif

// GPU�ɑ���萔�f�[�^

// ��{���
struct Utility
{
	float time;			// ����
};

// �V�[�����
struct SceneStruct
{
	// �J�����̃r���[�s��
	matrix view;
	// �J�����̃v���W�F�N�V�����s��
	matrix proj;
	// �J�����v���W�F�N�V�����̋t�s��
	matrix invProj;
	// �e�p�s��
	matrix shadow;
	// �J�����̉�]�s��
	matrix cameraRotate;
	// ���C�g�̍s��
	matrix lightCamera;
	// ���C�g�̌���
	float3 lightVec;
	float dummy0;

	// ���_
	float3 eye;
	// ����p
	float fov;
};