#pragma once
#include <memory>
#include <vector>
#include <array>
#include <d3d12.h>
#include <list>
#include <map>
#include <string>
#include "Utility/ComPtr.h"

class Renderer;
class Dx12Wrapper;
class ModelEndRendering;
class Mesh;
class VMDMotion;
class Actor;

class RendererManager
{
public:
	enum class RenderTargetType
	{
		color,
		normal,
		bright,
		max
	};

	RendererManager(Dx12Wrapper& dx12);
	~RendererManager();

	void Update();
	void Draw();

private:
	Dx12Wrapper& dx12_;

	struct MeshRender
	{
		std::shared_ptr<Renderer> renderer;
		std::vector<std::shared_ptr<Mesh>> meshs_;
	};
	std::vector<MeshRender> meshRenderers_;

	int cameraScreenH_;
	int shrinkScreenH_;

	ComPtr<ID3D12DescriptorHeap> renderTargetHeap_;
	std::array<int, static_cast<uint64_t>(RenderTargetType::max)> rendetTargetHandles_;

	std::shared_ptr<ModelEndRendering> modelEndrendering_;
	std::vector<std::shared_ptr<Actor>> actors_;

	void CreateRenderTargetHeap();

	// モーション情報の取得(モーションファイルパス)
	VMDMotion& GetVMDMotion(std::string motionPath);

	std::map<std::string, std::shared_ptr<VMDMotion>> vmdMotions_;
};