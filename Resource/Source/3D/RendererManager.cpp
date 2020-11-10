#include "RendererManager.h"
#include "Model/ModelRenderer.h"
#include "Primitive/PrimitiveRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "Utility/Constant.h"
#include "System/Application.h"
#include "3D/Camera.h"

using namespace std;

RendererManager::RendererManager(Dx12Wrapper& dx12):dx12_(dx12)
{
	renderers_.emplace_back(make_shared<ModelRenderer>(dx12_));
	renderers_.emplace_back(make_shared<PrimitiveRenderer>(dx12_));

	auto wsize = Application::Instance().GetWindowSize();
	auto& texLoader = dx12_.GetTexLoader();
	cameraScreenH_	= texLoader.MakeScreen(D3D_CAMERA_VIEW_SCREEN, wsize.w, wsize.h);
	lightScreenH_	= texLoader.MakeScreen(D3D_LIGHT_VIEW_SCREEN, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
}

RendererManager::~RendererManager()
{
}

void RendererManager::Update()
{
	dx12_.GetCamera().Update();
	for (auto& renderer : renderers_)
	{
		renderer->Update();
	}
}

void RendererManager::Draw()
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texLoader.SetDrawScreen(lightScreenH_, TexLoader::DepthType::light);
	texLoader.ClsDrawScreen();

	dx12_.SetDefaultViewAndScissor();

	for (auto& renderer : renderers_)
	{
		renderer->DrawShadow();
	}

	texLoader.SetDrawScreen(cameraScreenH_, TexLoader::DepthType::camera);
	texLoader.ClsDrawScreen();

	dx12_.SetDefaultViewAndScissor();

	for (auto& renderer : renderers_)
	{
		renderer->Draw();
	}
}