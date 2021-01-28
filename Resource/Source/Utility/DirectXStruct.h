#pragma once
#include <d3d12.h>
#include "Utility/ComPtr.h"

class Command;

// DirectX���\�[�X
struct Resource
{
	// �o�b�t�@
	ComPtr<ID3D12Resource> buffer;
	// ���
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	/// <summary>
	/// �o�b�t�@�Ƀo���A(��ԕύX)������
	/// </summary>
	/// <param name="cmd">�R�}���h�Ǘ��N���X</param>
	/// <param name="changeState">�ύX������</param>
	/// <returns>����</returns>
	bool Barrier(Command& cmd, const D3D12_RESOURCE_STATES changeState);
};

/// <summary>
/// �q�[�v�Ƀo�C���h�ς݂̃��\�[�X
/// </summary>
struct ResourceBindHeap
{
	// ���\�[�X
	Resource resource = {};
	// CPU�q�[�v�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE cpuH = {};
	// GPU�q�[�v�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE gpuH = {};
};