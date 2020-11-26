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

	// 初期化
	bool Init();
	// 描画
	void Draw(std::vector<std::shared_ptr<Mesh>>& models)override;
	// 影描画
	void DrawShadow(std::vector<std::shared_ptr<Mesh>>& models)override;

	// RootSignatureの設定
	void SetModelRS();
	// PipelineStateの設定
	void SetModelPL();

private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// 影用パイプラインステート
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	bool CreateModelPL();
};

