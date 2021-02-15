#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"
#include "Shader/Struct/MaterialBase.h"

/// <summary>
/// �}�e���A���N���X(����2D�y���p)
/// </summary>
class Material
{
public:
	~Material();

	/// <summary>
	/// �V�F�[�_�[�t�@�C���o�X�̎擾
	/// </summary>
	const std::wstring& GetShaderPath()const;

	/// <summary>
	/// �X�̃f�X�N���v�^�[�q�[�v�̐ݒ�
	/// </summary>
	void SetEachDescriptorHeap(ID3D12GraphicsCommandList& cmdList);

	/// <summary>
	/// �p�C�v���C���X�e�[�g�̎擾
	/// </summary>
	ComPtr<ID3D12PipelineState>& GetPipelineState();

	/// <summary>
	/// �}�e���A���x�[�X�̎擾
	/// </summary>
	/// <param name="index">�z��C���f�b�N�X</param>
	const MaterialBase& GetMaterialBase(const size_t index)const;

	/// <summary>
	/// �}�e���A���x�[�X�̐ݒ�
	/// </summary>
	/// <param name="index">�z��C���f�b�N�X</param>
	/// <param name="value">�ݒ肷��l</param>
	void SetMaterialBase(const size_t index, const MaterialBase& value);
	
	/// <summary>
	/// �ǉ��e�N�X�`���C���f�b�N�X�̐ݒ�
	/// </summary>
	/// <param name="index">�z��C���f�b�N�X</param>
	/// <param name="value">�ݒ肷��l</param>
	void SetAddTexIndex	(const size_t index, const int& value);

	/// <summary>
	/// �萔float�̐ݒ�
	/// </summary>
	/// <param name="index">�z��C���f�b�N�X</param>
	/// <param name="value">�ݒ肷��l</param>
	void SetConstFloat	(const size_t index, const float& value);

protected:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="shaderPaht">�V�F�[�_�[�t�@�C���p�X</param>
	Material(const std::wstring& shaderPaht);

	template<class T>
	struct StructuredResource
	{
		Resource resource = {};
		std::vector<T> elements;
		T* mapped = nullptr;
		D3D12_GPU_DESCRIPTOR_HANDLE handle = {};
	};

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				addTexIndexResource_;
	StructuredResource<float>			constFloatResource_;

	/// <summary>
	/// �f�[�^�����܂�����o�b�t�@�ƃq�[�v���쐬����
	/// </summary>
	void CreateEachDataBuffer();

private:
	const std::wstring shaderPath_;
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> heap_ = nullptr;

};