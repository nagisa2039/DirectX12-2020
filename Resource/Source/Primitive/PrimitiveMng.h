#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include <wrl.h>

class PrimitiveMesh;
class Dx12Wrapper;
using Microsoft::WRL::ComPtr;

class PrimitiveMng
{
private:
	Dx12Wrapper& _dx12;
	std::vector<std::shared_ptr<PrimitiveMesh>> _primitives;

	ComPtr<ID3D12RootSignature> _primRS = nullptr;
	ComPtr<ID3D12PipelineState> _primPL = nullptr;
	ComPtr<ID3D12PipelineState> _primShadowPL = nullptr;

	// �p�C�v���C���X�e�[�g�̍쐬
	bool CreatePipelineState();
	// ���[�g�V�O�l�`���̍쐬
	bool CreateRootSignature();

public:
	// (dx12�Ǘ��N���X)
	PrimitiveMng(Dx12Wrapper& dx12);
	~PrimitiveMng();

	// RootSignature��PipelineState�̐ݒ�
	void SetRSAndPL(bool shadow = false);

	// �X�V
	void Update();
	// �`��
	void Draw();
};

