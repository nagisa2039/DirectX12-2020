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

/// <summary>
/// SkeletalMesh�̕`��N���X
/// </summary>
class SkeletalMeshRenderer : 
	public Renderer
{
public:
	/// <param name="dx12">IrectX�Ǘ��N���X</param>
	SkeletalMeshRenderer(Dx12Wrapper& dx12);
	~SkeletalMeshRenderer();

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Init();

	/// <summary>
	/// �ʏ�`��
	/// </summary>
	/// <param name="meshs">�`�悷�郁�b�V��</param>
	void Draw(std::vector<Mesh*>& meshs)override;

	/// <summary>
	/// �e�`��
	/// </summary>
	/// <param name="meshs">�`�悷�郁�b�V��</param>
	void DrawShadow(std::vector<Mesh*>& meshs)override;


private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// �e�p�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	bool CreateModelPL();
	void SetModelRS();
	void SetModelPL();
};

