#pragma once
#include <memory>
#include <vector>
#include <array>
#include <d3d12.h>
#include <list>
#include "Utility/ComPtr.h"

class Renderer;
class Dx12Wrapper;
class ModelEndRendering;

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
	std::vector<std::shared_ptr<Renderer>> renderers_;

	int cameraScreenH_;
	int lightScreenH_;

	ComPtr<ID3D12DescriptorHeap> renderTargetHeap_;
	std::array<int, static_cast<uint64_t>(RenderTargetType::max)> rendetTargetHandles_;

	std::shared_ptr<ModelEndRendering> modelEndrendering_;

	void CreateRenderTargetHeap();
};