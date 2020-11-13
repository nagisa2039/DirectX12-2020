#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
using float4 = DirectX::XMFLOAT4;
using uint = uint32_t;
using min16uint = uint16_t;
#endif

struct MaterialStruct
{
	float4 diffuse;//�g�U����
	float4 specular;//���ʔ���
	float4 ambient;//��������
	float power;	// �X�y�L�����搔

	// �e�N�X�`��
	int texIdx;
	// �X�t�B�A�}�b�v	��Z
	int sphIdx;
	// �X�t�B�A�}�b�v	���Z
	int spaIdx;
	// �ǉ��e�N�X�`��
	int addtexIdx;
	// toon�e�N�X�`��
	int toonIdx;
};

struct MaterialIndex
{
	uint index;
};