#pragma once
#include <DirectXTex.h>
#include "Utility/DirectXStruct.h"

// ���e�N�X�`���n���h��
struct DummyTextures
{
	// ��
	int whiteTexH;
	// ��
	int blackTexH;
	// �ォ�牺�ւ̃O���f�[�V����
	int gradTexH;
};

// �e�N�X�`�����\�[�X
struct TextureResorce
{
	// �摜���
	DirectX::Image imageInf = {};
	// ���\�[�X
	Resource resource;
	// GPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleForTex = {};
	// CPU�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleForRtv = {};
};