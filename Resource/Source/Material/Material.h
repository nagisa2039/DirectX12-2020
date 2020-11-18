#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"
#include "MaterialBase.h"

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
	/// �}�e���A���z��̎擾
	/// </summary>
	std::vector <MaterialBase>& GetMaterialBaseVec();

	/// <summary>
	/// �ǉ��e�N�X�`���C���f�b�N�X�z��̎擾
	/// </summary>
	std::vector<int>& GetAddTextureIndexVec();

	/// <summary>
	/// float�萔�z��̎擾
	/// </summary>
	std::vector<float>& GetConstFloatVec();

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
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
	};

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				texIndexResource_;
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