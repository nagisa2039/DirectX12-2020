#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include "3D/Renderer.h"

class PrimitiveMesh;
class Dx12Wrapper;
using Microsoft::WRL::ComPtr;

class PrimitiveRenderer : 
	public Renderer
{
private:
	Dx12Wrapper& dx12_;
	std::vector<std::shared_ptr<PrimitiveMesh>> primitives_;

	ComPtr<ID3D12RootSignature> primRS_ = nullptr;
	ComPtr<ID3D12PipelineState> primPL_ = nullptr;
	ComPtr<ID3D12PipelineState> primShadowPL_ = nullptr;

	// �p�C�v���C���X�e�[�g�̍쐬
	bool CreatePipelineState();

public:
	// (dx12�Ǘ��N���X)
	PrimitiveRenderer(Dx12Wrapper& dx12);
	~PrimitiveRenderer();

	// �X�V
	void Update()override;
	// �`��
	void Draw()override;
	// �e�`��
	void DrawShadow()override;
};

