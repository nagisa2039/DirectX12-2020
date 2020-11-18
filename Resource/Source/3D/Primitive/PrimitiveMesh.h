#pragma once
#include <DirectXMath.h>
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>

class Dx12Wrapper;
class Material;

class PrimitiveMesh
{
public:
	struct PrimVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		PrimVertex(): 
			pos(DirectX::XMFLOAT3(0,0,0)), 
			normal(DirectX::XMFLOAT3(0, 0, 0)),
			uv(DirectX::XMFLOAT2(0.0f, 0.0f)){};

		PrimVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 norm, DirectX::XMFLOAT2 uv) :
			pos(p), normal(norm), uv(uv){};

		PrimVertex(float x, float y, float z, float nx, float ny, float nz) :
			pos(DirectX::XMFLOAT3(x, y, z)),
			normal(DirectX::XMFLOAT3(nx, ny, nz)),
			uv(DirectX::XMFLOAT2(0.0f, 0.0f)) {};
	};

	// (dx12�̊Ǘ��N���X, ���W, �e�N�X�`���t�@�C���p�X)
	PrimitiveMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath = L"");
	~PrimitiveMesh();
	// �X�V
	virtual void Update();
	// �`��
	virtual void Draw();

protected:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12Resource>texBuffer_;
	ComPtr<ID3D12DescriptorHeap>texHeap_;

	ComPtr<ID3D12Resource> vbuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	ComPtr<ID3D12Resource> ibuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

	int indexNum_;

	std::unique_ptr<Material> material_;

	// ���W
	DirectX::XMFLOAT3 pos_;
	DirectX::XMMATRIX* mappedTrans_;

	// ���W�s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> transCB_ = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> worldHeap_ = nullptr;

	// ��]
	DirectX::XMFLOAT3 rotate_;

	// �A�j���[�V�����J�E���g
	unsigned int animCnt_;

	 // ���_�o�b�t�@�ƃr���[�̍쐬
	void CreateVertexBufferAndView(std::vector<PrimVertex> vertices);
	// �C���f�b�N�X�o�b�t�@�ƃr���[�̍쐬
	void CreateIndexBufferAndView(std::vector<uint16_t> indices);
	// transform�o�b�t�@�̍쐬
	void CreateTransBuffer();

	// �e���_�̖@���v�Z(���_�z��, �C���f�b�N�X�z��)
	void CalNormalVertex(std::vector<PrimVertex>& vertices, const std::vector<uint16_t>& indices);
	// �O�p�`�̕\�ʖ@���̎擾(���_0, ���_1, ���_2)
	DirectX::XMVECTOR GetSurfaceNormal(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
	// �O�p�`�̕\�ʖ@���̎擾(���_�z��, ���S)
	void CalNormalVertex2(std::vector<PrimVertex>& vertices, DirectX::XMFLOAT3 center);
};

