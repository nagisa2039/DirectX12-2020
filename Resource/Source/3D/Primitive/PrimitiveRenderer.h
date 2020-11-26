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

	ComPtr<ID3D12RootSignature> primRS_ = nullptr;
	ComPtr<ID3D12PipelineState> primPL_ = nullptr;
	ComPtr<ID3D12PipelineState> primShadowPL_ = nullptr;

	// パイプラインステートの作成
	bool CreatePipelineState();

public:
	// (dx12管理クラス)
	PrimitiveRenderer(Dx12Wrapper& dx12);
	~PrimitiveRenderer();

	// 描画
	void Draw(std::vector<std::shared_ptr<Mesh>>& models)override;
	// 影描画
	void DrawShadow(std::vector<std::shared_ptr<Mesh>>& models)override;
};

