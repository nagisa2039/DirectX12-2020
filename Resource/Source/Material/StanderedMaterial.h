#pragma once
#include "Material.h"

/// <summary>
/// ��{�}�e���A��
/// �ǉ���񂪕s�K�v�ȃV�F�[�_�[�Ɏg�p
/// </summary>
class StanderedMaterial :
    public Material
{
public:
	/// <param name="shaderPath">�V�F�[�_�[�p�X</param>
	StanderedMaterial(const std::wstring& shaderPath);
	~StanderedMaterial();
};

