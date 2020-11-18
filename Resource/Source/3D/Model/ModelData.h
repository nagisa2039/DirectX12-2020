#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

class ModelData
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
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
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

	ModelData();
	~ModelData();

	// ���_���̎擾
	std::vector<Vertex>& GetVertexData();
	// �C���f�b�N�X���̎擾
	std::vector<uint32_t>& GetIndexData();
	// �e�N�X�`���p�X���̎擾
	std::vector<MultiTexturePath>& GetTexturePaths();
	// �}�e���A�����̎擾
	std::vector<Material>& GetMaterialData();
	//�}�e���A���C���f�b�N�X
	std::vector<unsigned int>& GetMaterialIndexData();
	// �{�[�����̎擾
	std::vector<Bone>& GetBoneData();

protected:

	std::vector<Vertex> vertexData_;	// ���_�f�[�^
	std::vector<uint32_t> indexData_;	// ���_�C���f�b�N�X�f�[�^
	std::vector<MultiTexturePath> texPaths_;		//�e�N�X�`���̃p�X(����)
	std::vector<Material> materials_;		// �}�e���A���f�[�^
	std::vector<Bone> bones_;
	std::vector<unsigned int> materialIndexData_;	// �}�e���A���̃C���f�b�N�X ���_�C���f�b�N�X�ɑΉ�

	/// <summary>
	/// �}�e���A����ǂݍ��񂾌��indecesNum�����
	/// Vertex��materialIdx��ݒ肷��
	/// </summary>
	void SetVertexMaterialIndex();
};

