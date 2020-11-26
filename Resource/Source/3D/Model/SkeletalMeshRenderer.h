#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "3D/Renderer.h"

using Microsoft::WRL::ComPtr;
class SkeletalMesh;
class Dx12Wrapper;
class Mesh;

class SkeletalMeshRenderer : 
	public Renderer
{
public:
	SkeletalMeshRenderer(Dx12Wrapper& dx12);
	~SkeletalMeshRenderer();

	// ������
	bool Init();
	// �`��
	void Draw(std::vector<std::shared_ptr<Mesh>>& models)override;
	// �e�`��
	void DrawShadow(std::vector<std::shared_ptr<Mesh>>& models)override;

	// RootSignature�̐ݒ�
	void SetModelRS();
	// PipelineState�̐ݒ�
	void SetModelPL();

private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// �e�p�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	bool CreateModelPL();
};

