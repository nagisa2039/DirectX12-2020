#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "3D/Renderer.h"

using Microsoft::WRL::ComPtr;
class SkeletalMesh;
class Dx12Wrapper;

/// <summary>
/// SkeletalMeshの描画クラス
/// </summary>
class SkeletalMeshRenderer : 
	public Renderer
{
public:
	/// <param name="dx12">DirectX管理クラス</param>
	/// <param name="camera">カメラクラス</param>
	SkeletalMeshRenderer(Dx12Wrapper& dx12, std::shared_ptr<Camera>& camera);
	~SkeletalMeshRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Init();

	/// <summary>
	/// Computeによる更新
	/// Rendererから呼ぶためにUpdateから分離
	/// </summary>
	void ComputeUpdate(std::vector<Mesh*>& meshs)override;

	/// <summary>
	/// 通常描画
	/// </summary>
	/// <param name="meshs">描画するメッシュ</param>
	void Draw(std::vector<Mesh*>& meshs)override;

	/// <summary>
	/// 影描画
	/// </summary>
	/// <param name="meshs">描画するメッシュ</param>
	void DrawShadow(std::vector<Mesh*>& meshs)override;


private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// 影用パイプラインステート
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	// Compute用
	ComPtr<ID3D12RootSignature> computeRS_ = nullptr;
	ComPtr<ID3D12PipelineState> computePL_ = nullptr;

	bool CreateModelPL();
	bool CreateCompute();
	void SetModelRS();
	void SetModelPL();
};

