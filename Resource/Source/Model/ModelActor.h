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
#include "Utility/TextureStruct.h"

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
	struct MaterialForBuffer
	{
		DirectX::XMFLOAT4 diffuse;//�g�U����
		DirectX::XMFLOAT4 specular;//���ʔ���
		DirectX::XMFLOAT4 ambient;//��������
		float power;	// �X�y�L�����搔
	
		// �e�N�X�`��
		uint16_t texIdx;
		// �X�t�B�A�}�b�v	��Z
		uint16_t sphIdx;
		// �X�t�B�A�}�b�v	���Z
		uint16_t spaIdx;
		// �ǉ��e�N�X�`��
		uint16_t addtexIdx;
		// toon�e�N�X�`��
		uint16_t toonIdx;
	};

	ModelRenderer& renderer_;
	Dx12Wrapper& dx12_;


	std::shared_ptr<ModelData> modelData_;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};


	// ���_�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// GPU�ɓ]���p�}�e���A���z��
	std::vector<MaterialForBuffer> mats_;

	// �}�e���A���o�b�t�@
	ComPtr<ID3D12Resource> materialBuffer_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> materialHeap_ = nullptr;

	// �}�e���A���C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> materialIndexBuffer_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> materialIndexHeap_ = nullptr;

	Transform trans_;
	DirectX::XMMATRIX* mappedTrans_;

	// ���W�s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> transCB_ = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> worldHeap_ = nullptr;

	struct BoneNode
	{
		int boneIdx = 0;	// _boneMats�̃C���f�b�N�X
		DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̎n�_
		DirectX::XMFLOAT3 endPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// �{�[���̏I�_
		std::vector<BoneNode*> children; // �q�������ւ̃����N
	};

	std::vector<DirectX::XMMATRIX> boneMats_;	// �e�{�[����transform�����i�[
	std::unordered_map<std::wstring, BoneNode> boneMap_;	// �{�[��������boneMats�̃C���f�b�N�X���Ƃ�

	ComPtr<ID3D12Resource> boneCB_ = nullptr;
	DirectX::XMMATRIX* mappedBones_;

	//vmd
	VMDMotion& vmdMotion_;

	uint64_t lastTickTime_;

	// ���_buffer�̍쐬
	bool CreateVertexBuffer();
	// ���_�C���f�b�N�X�o�b�t�@�̍쐬
	bool CreateIndexBuffer();

	// �}�e���A���o�b�t�@�̍쐬
	//bool CreateMaterialBuffer();
	// �}�e���A���̃e�N�X�`���o�b�t�@�쐬�쐬
	//bool CreateMaterialTextureBuffer();
	// �}�e���A����CBV�쐬
	bool CreateMaterial();
	// �}�e���A���̃e�N�X�`���r���[�̍쐬
	//void CreateMaterialTextureView(D3D12_CONSTANT_BUFFER_VIEW_DESC& viewDesc, D3D12_CPU_DESCRIPTOR_HANDLE& handle, const UINT64& heapStride, const UINT64& bufferStride, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

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