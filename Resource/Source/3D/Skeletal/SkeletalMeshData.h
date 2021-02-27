#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Shader/Struct/SkeletalMeshVertex.h"

/// <summary>
/// �{�[���L���f���f�[�^
/// </summary>
class SkeletalMeshData
{
public :
	// �}�e���A��
	struct Material
	{
		DirectX::XMFLOAT4 diffuse;	// �f�B�t���[�Y�F
		DirectX::XMFLOAT3 specular;	// ���ʔ��ˌ�
		DirectX::XMFLOAT3 ambient;	// ���\����
		float power;	// �X�y�L�����搔
		uint32_t indeicesNum;	// �C���f�b�N�X��
	};

	// �}�e���A�����Ƃ̃e�N�X�`���p�X
	struct MultiTexturePath
	{
		std::wstring texPath;	// �ʏ�e�N�X�`���p�X
		std::wstring sphPath;	// ��Z�e�N�X�`���p�X
		std::wstring spaPath;	// ���Z�e�N�X�`���p�X
		std::wstring subPath;	// �T�u�e�N�X�`��
		std::wstring toonPath;	// toon�e�N�X�`���p�X
	};

	// �{�[�����
	struct Bone
	{
		std::wstring name;
		int parentIdx = 0;
		DirectX::XMFLOAT3 startPos  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̎n�_
		DirectX::XMFLOAT3 endPos	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̏I�_
	};

	SkeletalMeshData();
	~SkeletalMeshData();

	/// <summary>
	/// ���_���̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<SkeletalMeshVertex>& GetVertexData();

	/// <summary>
	/// �C���f�b�N�X���̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<uint32_t>& GetIndexData();

	/// <summary>
	/// �e�N�X�`���p�X���̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<MultiTexturePath>& GetTexturePaths();

	/// <summary>
	/// �}�e���A�����̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<Material>& GetMaterialData();

	/// <summary>
	/// �{�[�����̎擾
	/// </summary>
	/// <returns></returns>
	const std::vector<Bone>& GetBoneData();

protected:

	std::vector<SkeletalMeshVertex> vertexData_;	// ���_�f�[�^
	std::vector<uint32_t> indexData_;	// ���_�C���f�b�N�X�f�[�^
	std::vector<MultiTexturePath> texPaths_;		//�e�N�X�`���̃p�X(����)
	std::vector<Material> materials_;		// �}�e���A���f�[�^
	std::vector<Bone> bones_;
};

