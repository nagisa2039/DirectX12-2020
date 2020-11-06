#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "3D/Renderer.h"

using Microsoft::WRL::ComPtr;
class ModelActor;
class Dx12Wrapper;
class VMDMotion;

class ModelRenderer : 
	public Renderer
{
public:
	ModelRenderer(Dx12Wrapper& dx12);
	~ModelRenderer();

	// 初期化
	bool Init();
	// 更新
	void Update()override;
	// 描画
	void Draw()override;
	// 影描画
	void DrawShadow()override;

	// RootSignatureの設定
	void SetModelRS();
	// PipelineStateの設定
	void SetModelPL();
	// モーション情報の取得(モーションファイルパス)
	VMDMotion& GetVMDMotion(std::string motionPath);

private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// 影用パイプラインステート
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	std::vector<std::shared_ptr<ModelActor>> modelActors_;
	std::map<std::string, std::shared_ptr<VMDMotion>> vmdMotions_;

	bool CreateModelPL();
};

