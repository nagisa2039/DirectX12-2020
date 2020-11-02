#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <map>
#include <string>

using Microsoft::WRL::ComPtr;
class ModelActor;
class Dx12Wrapper;
class VMDMotion;


class ModelRenderer
{
public:
	ModelRenderer(Dx12Wrapper& dx12);
	~ModelRenderer();

	// 初期化
	bool Init();
	// 更新
	void Update();
	// 描画
	void Draw();

	void DrawTo3DSpace();

	// RootSignatureの設定
	void SetModelRS();
	// PipelineStateの設定
	void SetModelPL();
	// 影の描画
	void DrawFramShadow();
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

	int screenH_;
	int lightScreenH_;

	bool CreateModelPL();
};

