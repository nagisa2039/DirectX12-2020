#pragma once
#include "3D/Mesh.h"
#include <DirectXMath.h>
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>

class Material;

class StaticMesh :
	public Mesh
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
	StaticMesh(std::weak_ptr<Actor>owner, Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath = L"");
	~StaticMesh();
	// �X�V
	void Update()override;
	// �`��
	void Draw()override;

protected:
	ComPtr<ID3D12Resource> vbuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	ComPtr<ID3D12Resource> ibuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

	int indexNum_;

	std::unique_ptr<Material> material_;

	// �A�j���[�V�����J�E���g
	unsigned int animCnt_;

	 // ���_�o�b�t�@�ƃr���[�̍쐬
	void CreateVertexBufferAndView(std::vector<PrimVertex> vertices);
	// �C���f�b�N�X�o�b�t�@�ƃr���[�̍쐬
	void CreateIndexBufferAndView(std::vector<uint16_t> indices);

	// �e���_�̖@���v�Z(���_�z��, �C���f�b�N�X�z��)
	void CalNormalVertex(std::vector<PrimVertex>& vertices, const std::vector<uint16_t>& indices);
	// �O�p�`�̕\�ʖ@���̎擾(���_0, ���_1, ���_2)
	DirectX::XMVECTOR GetSurfaceNormal(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
	// �O�p�`�̕\�ʖ@���̎擾(���_�z��, ���S)
	void CalNormalVertex2(std::vector<PrimVertex>& vertices, DirectX::XMFLOAT3 center);
};

