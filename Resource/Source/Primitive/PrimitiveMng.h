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

	// パイプラインステートの作成
	bool CreatePipelineState();
	// ルートシグネチャの作成
	bool CreateRootSignature();

public:
	// (dx12管理クラス)
	PrimitiveMng(Dx12Wrapper& dx12);
	~PrimitiveMng();

	// RootSignatureとPipelineStateの設定
	void SetRSAndPL(bool shadow = false);

	// 更新
	void Update();
	// 描画
	void Draw();
};

