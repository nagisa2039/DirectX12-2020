#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <string>

class Dx12Wrapper;
using Microsoft::WRL::ComPtr;

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

protected:
	Dx12Wrapper& _dx12;

	ComPtr<ID3D12Resource>_texBuffer;
	ComPtr<ID3D12DescriptorHeap>_texHeap;

	ComPtr<ID3D12Resource> _vbuffer;
	D3D12_VERTEX_BUFFER_VIEW _vbv;

	ComPtr<ID3D12Resource> _ibuffer;
	D3D12_INDEX_BUFFER_VIEW _ibv;

	int _indexNum;

	// ���W
	DirectX::XMFLOAT3 _pos;
	DirectX::XMMATRIX* _mappedTrans;

	// ���W�s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> _transCB = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> _worldHeap = nullptr;

	// ��]
	DirectX::XMFLOAT3 _rotate;

	// �A�j���[�V�����J�E���g
	unsigned int _animCnt;

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

public:
	// (dx12�̊Ǘ��N���X, ���W, �e�N�X�`���t�@�C���p�X)
	PrimitiveMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath = L"");
	~PrimitiveMesh();
	// �X�V
	virtual void Update();
	// �`��
	virtual void Draw();
};
