#pragma once
#include <memory>
#include <vector>
#include <array>
#include <d3d12.h>
#include <string>
#include "Utility/ComPtr.h"

class Renderer;
class Dx12Wrapper;
class ModelEndRendering;
class Mesh;
class CameraObject;

/// <summary>
/// 3Dメッシュの描画管理クラス
/// </summary>
class RendererManager
{
public:
	// マルチパスレンダリングの種類
	enum class RenderTargetType
	{
		color,
		normal,
		bright,
		max
	};

	/// <param name="dx12">DirectX12管理クラス</param>
	RendererManager(Dx12Wrapper& dx12);
	~RendererManager();

	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// メッシュの追加
	/// </summary>
	/// <param name="mesh"></param>
	void AddMesh(Mesh* mesh);

	/// <summary>
	/// メッシュの削除
	/// </summary>
	/// <param name="mesh"></param>
	void RemoveMesh(Mesh* mesh);

private:
	Dx12Wrapper& dx12_;

	struct MeshRender
	{
		std::shared_ptr<Renderer> renderer;
		std::vector<Mesh*> meshs_;
	};
	std::vector<MeshRender> meshRenderers_;

	int cameraScreenH_;
	int shrinkScreenH_;

	ComPtr<ID3D12DescriptorHeap> renderTargetHeap_;
	std::array<int, static_cast<uint64_t>(RenderTargetType::max)> rendetTargetHandles_;

	std::shared_ptr<ModelEndRendering> modelEndrendering_;
	std::shared_ptr<CameraObject> camera_;

	void CreateRenderTargetHeap();
};