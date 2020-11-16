#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"

/// <summary>
/// �}�e���A���N���X(����2D�y���p)
/// </summary>
class Material
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="shaderPaht">�V�F�[�_�[�t�@�C���p�X</param>
	Material(const std::wstring& shaderPaht);
	~Material();

	const std::wstring& GetShaderPath()const;

	/// <summary>
	/// �X�̃f�X�N���v�^�[�q�[�v�̐ݒ�
	/// </summary>
	void SetEachDescriptorHeap(ID3D12GraphicsCommandList& cmdList);

private:
	const std::wstring shaderPath_;
	std::vector<int> textureIndexVec_;
	std::vector<float> constantFloatVec_;

	Resource texIndexResource;
	int* mappedTexIndex;
	ComPtr<ID3D12DescriptorHeap> textureIndexHeap_;
	Resource constantFloatResource;
	float* mappedConstantFloat;
	ComPtr<ID3D12DescriptorHeap> constantFloatHeap_;

	void CreateEachDataBuffer();

	/// <summary>
	/// �X�̃f�[�^���쐬
	/// </summary>
	virtual void AddEachData() = 0;
};

