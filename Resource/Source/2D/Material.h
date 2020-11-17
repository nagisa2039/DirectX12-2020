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
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="shaderPaht">�V�F�[�_�[�t�@�C���p�X</param>
	Material(const std::wstring& shaderPaht);
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

protected:
	MaterialBase& GetMaterialBase();
	std::vector<int>& GetAddTextureIndexVec();
	std::vector<float>& GetConstFloatVec();

	/// <summary>
	/// �X�̃f�[�^���쐬
	/// </summary>
	//virtual void AddEachData() = 0;
	void CreateEachDataBuffer();

private:
	const std::wstring shaderPath_;

	template<class T>
	struct StructuredResource
	{
		Resource resource = {};
		std::vector<T> elements;
		T* mapped = nullptr;
		ComPtr<ID3D12DescriptorHeap> heap = nullptr;
	};

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				texIndexResource_;
	StructuredResource<float>			constFloatResource_;


};