#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

class SkeletalMeshData
{
public :

	struct Material
	{
		DirectX::XMFLOAT4 diffuse;	// �f�B�t���[�Y�F
		DirectX::XMFLOAT3 specular;	// ���ʔ��ˌ�
		DirectX::XMFLOAT3 ambient;	// ���\����
		float power;	// �X�y�L�����搔
		uint32_t indeicesNum;	// �C���f�b�N�X��
	};

	struct MultiTexturePath
	{
		std::wstring texPath;	// �ʏ�e�N�X�`���p�X
		std::wstring sphPath;	// ��Z�e�N�X�`���p�X
		std::wstring spaPath;	// ���Z�e�N�X�`���p�X
		std::wstring subPath;	// �T�u�e�N�X�`��
		std::wstring toonPath;	// toon�e�N�X�`���p�X
	};


	struct Vertex
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT2 uv;
		DirectX::XMINT4 boneIdx;
		DirectX::XMFLOAT4 weight;
	};

	struct Bone
	{
		std::wstring name;
		int parentIdx = 0;
		DirectX::XMFLOAT3 startPos  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̎n�_
		DirectX::XMFLOAT3 endPos	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̏I�_
	};

	SkeletalMeshData();
	~SkeletalMeshData();

	// ���_���̎擾
	const std::vector<Vertex>& GetVertexData();
	// �C���f�b�N�X���̎擾
	const std::vector<uint32_t>& GetIndexData();
	// �e�N�X�`���p�X���̎擾
	const std::vector<MultiTexturePath>& GetTexturePaths();
	// �}�e���A�����̎擾
	const std::vector<Material>& GetMaterialData();
	// �{�[�����̎擾
	const std::vector<Bone>& GetBoneData();

protected:

	std::vector<Vertex> vertexData_;	// ���_�f�[�^
	std::vector<uint32_t> indexData_;	// ���_�C���f�b�N�X�f�[�^
	std::vector<MultiTexturePath> texPaths_;		//�e�N�X�`���̃p�X(����)
	std::vector<Material> materials_;		// �}�e���A���f�[�^
	std::vector<Bone> bones_;
};

