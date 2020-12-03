#pragma once
#include <vector>
#include <memory>
#include <d3d12.h>
#include "Utility/ComPtr.h"
#include "3D/Renderer.h"

class Dx12Wrapper;

/// <summary>
/// StaticMeshの描画クラス
/// </summary>
class StaticMeshRenderer : 
	public Renderer
{
public:
	/// <param name="dx12">DirectX管理クラス</param>
	/// <param name="camera">カメラクラス</param>
	StaticMeshRenderer(Dx12Wrapper& dx12, std::shared_ptr<Camera>& camera);
	~StaticMeshRenderer();

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

	ComPtr<ID3D12RootSignature> primRS_ = nullptr;
	ComPtr<ID3D12PipelineState> primPL_ = nullptr;
	ComPtr<ID3D12PipelineState> primShadowPL_ = nullptr;

	// パイプラインステートの作成
	bool CreatePipelineState();
};

