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
/// SkeletalMeshの描画クラス
/// </summary>
class SkeletalMeshRenderer : 
	public Renderer
{
public:
	/// <param name="dx12">IrectX管理クラス</param>
	SkeletalMeshRenderer(Dx12Wrapper& dx12);
	~SkeletalMeshRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Init();

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

	bool CreateModelPL();
	void SetModelRS();
	void SetModelPL();
};

