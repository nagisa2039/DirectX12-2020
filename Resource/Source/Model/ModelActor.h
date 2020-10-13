#pragma once
#include <vector>
#include <DirectXMath.h>
#include <string>
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include "ModelData.h"
#include <memory>
#include "System/TextureStruct.h"

class VMDMotion;
class Dx12Wrapper;
class ModelRenderer;
using Microsoft::WRL::ComPtr;

class ModelActor
{
public:
	struct Transform
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 rotate;
	};

	ModelActor(std::string modelPath, Dx12Wrapper& dx12, ModelRenderer& rnderer, VMDMotion& vmd);
	~ModelActor();

	// ������(���f���̃t�@�C���p�X)
	bool Init(std::string modelPath);
	// �X�V
	void Update();
	// �`��(�e�Ƃ��ĕ`�悷�邩)
	void Draw(bool isShadow = false);
	// �A�j���[�V�������J�n����
	void StartAnimation();
	// transform�擾
	Transform& GetTransform();
	// transform�̐ݒ�
	void SetTransform(const Transform& trans);

private:

	struct MultiTexture
	{
		TextureResorce texResource;	// �ʏ�e�N�X�`��
		TextureResorce sphResource;	// sph�e�N�X�`��
		TextureResorce spaResource;	// spa�e�N�X�`��
		TextureResorce subResource;	// �T�u�e�N�X�`��
		TextureResorce toonResource;	// toon�e�N�X�`��
	};

	struct MaterialForBuffer
	{
		DirectX::XMFLOAT4 diffuse;//�g�U����
		DirectX::XMFLOAT4 specular;//���ʔ���
		DirectX::XMFLOAT4 ambient;//��������
		float power;	// �X�y�L�����搔
	};

	ModelRenderer& _renderer;
	Dx12Wrapper& _dx12;


	std::shared_ptr<ModelData> _modelData;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};


	// ���_�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> _indexBuffer = nullptr;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW _ibView = {};


	// �e�N�X�`�����\�[�X
	std::vector<MultiTexture> _textures;

	// �}�e���A���o�b�t�@
	ComPtr<ID3D12Resource> _materialBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _materialHeap = nullptr;

	Transform _trans;
	DirectX::XMMATRIX* _mappedTrans;

	// ���W�s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> _transCB = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> _worldHeap = nullptr;


	struct BoneNode
	{
		int boneIdx = 0;	// _boneMats�̃C���f�b�N�X
		DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̎n�_
		DirectX::XMFLOAT3 endPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̏I�_
		std::vector<BoneNode*> children; // �q�������ւ̃����N
	};

	std::vector<DirectX::XMMATRIX> _boneMats;	// �e�{�[����transform�����i�[
	std::unordered_map<std::wstring, BoneNode> _boneMap;	// �{�[��������boneMats�̃C���f�b�N�X���Ƃ�

	ComPtr<ID3D12Resource> _boneCB = nullptr;
	DirectX::XMMATRIX* _mappedBones;

	//vmd
	VMDMotion& _vmdMotion;

	uint32_t _lastTickTime;

	// ���_buffer�̍쐬
	bool CreateVertexBuffer();
	// ���_�C���f�b�N�X�o�b�t�@�̍쐬
	bool CreateIndexBuffer();

	// �}�e���A���o�b�t�@�̍쐬
	bool CreateMaterialBuffer();
	// �}�e���A���̃e�N�X�`���o�b�t�@�쐬�쐬
	bool CreateMaterialTextureBuffer();
	// �}�e���A����CBV�쐬
	bool CreateMaterialCBV();
	// �}�e���A���̃e�N�X�`���r���[�̍쐬
	void CreateMaterialTextureView(D3D12_CONSTANT_BUFFER_VIEW_DESC& viewDesc, D3D12_CPU_DESCRIPTOR_HANDLE& handle, const UINT& heapStride, unsigned int bufferStride, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

	// �{�[���K�w�̍쐬
	bool CreateBoneHierarchy();

	// �s��v�Z�̍ċA�֐�
	void RecursiveMatrixMultiply(BoneNode& node, DirectX::XMMATRIX& inMat);

	// �{�[����]�p�֐�
	void RotateBone(std::wstring boneName, DirectX::XMVECTOR location, DirectX::XMFLOAT4& q1, DirectX::XMFLOAT4& q2, float t);

	// ���W�ƃ{�[���p�̃o�b�t�@�ƃr���[�쐬
	bool CreateConstanteBuffers();

	// �A�j���[�V�����̍X�V
	void MotionUpdate(const unsigned int motionFrame);
};