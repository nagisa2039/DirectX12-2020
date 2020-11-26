#pragma once
#include "3D/Mesh.h"
#include <vector>
#include <DirectXMath.h>
#include <string>
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include <memory>
#include "Utility/TextureStruct.h"
#include "Utility/TimeLine.h"
#include "SkeletalMeshData.h"

class VMDMotion;
class Dx12Wrapper;
class ModelMaterial;
using Microsoft::WRL::ComPtr;

class SkeletalMesh:
	public Mesh
{
public:
	SkeletalMesh(std::shared_ptr<Actor> owner, 
		std::string modelPath, Dx12Wrapper& dx12, VMDMotion& vmd);
	~SkeletalMesh();

	// ������(���f���̃t�@�C���p�X)
	bool Init(std::string modelPath);
	// �X�V
	void Update();
	// �`��(�e�Ƃ��ĕ`�悷�邩)
	void Draw();
	// �A�j���[�V�������J�n����
	void StartAnimation();

private:
	Dx12Wrapper& dx12_;

	std::shared_ptr<SkeletalMeshData> modelData_;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// ���_�C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// �}�e���A��
	std::unique_ptr<ModelMaterial> modelMaterial_;

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
	DirectX::XMMATRIX* mappedBones_ = nullptr;

	ComPtr<ID3D12DescriptorHeap> boneHeap_ = nullptr;

	//vmd
	VMDMotion& vmdMotion_;

	uint64_t lastTickTime_;

	std::unique_ptr<TimeLine<float>> noiseThresholdTL_;

	// ���_buffer�̍쐬
	bool CreateVertexBuffer();
	// ���_�C���f�b�N�X�o�b�t�@�̍쐬
	bool CreateIndexBuffer();

	// �}�e���A���̍쐬
	bool CreateMaterial();

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