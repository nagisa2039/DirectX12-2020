#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include "Utility/ComPtr.h"
#include "3D/Renderer.h"

class Mesh;
class Dx12Wrapper;

/// <summary>
/// StaticMesh�̕`��N���X
/// </summary>
class StaticMeshRenderer : 
	public Renderer
{
public:
	// (dx12�Ǘ��N���X)
	StaticMeshRenderer(Dx12Wrapper& dx12);
	~StaticMeshRenderer();

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

	ComPtr<ID3D12RootSignature> primRS_ = nullptr;
	ComPtr<ID3D12PipelineState> primPL_ = nullptr;
	ComPtr<ID3D12PipelineState> primShadowPL_ = nullptr;

	// �p�C�v���C���X�e�[�g�̍쐬
	bool CreatePipelineState();
};
