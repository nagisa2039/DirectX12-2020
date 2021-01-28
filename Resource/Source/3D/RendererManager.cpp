#include "RendererManager.h"
#include "Skeletal/SkeletalMeshRenderer.h"
#include "Static/StaticMeshRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "Utility/Constant.h"
#include "System/Application.h"
#include "3D/SceneInf.h"
#include "Utility/dx12Tool.h"
#include "2D/SpriteDrawer.h"
#include "Material/ModelEndRendering.h"
#include "Mesh.h"

using namespace std;
using namespace DirectX;

namespace
{
	constexpr unsigned int SHRINK_CNT = 4;
}

RendererManager::RendererManager(Dx12Wrapper& dx12) :dx12_(dx12)
{
	sceneInf_ = make_shared<SceneInf>(dx12.GetCommand(), dx12.GetDevice());
	sceneInf_->Init();
	meshRenderers_.resize(Uint64(Mesh::Type::max));
	meshRenderers_[Uint64(Mesh::Type::static_mesh)].renderer	= make_shared<StaticMeshRenderer>(dx12_, sceneInf_);
	meshRenderers_[Uint64(Mesh::Type::skeletal_mesh)].renderer	= make_shared<SkeletalMeshRenderer>(dx12_, sceneInf_);

	auto wsize = Application::Instance().GetWindowSize();
	auto& texLoader = dx12_.GetTexLoader();

	wstring screenNames[] = { SCR_CAMERA_MR_COLOR, SCR_CAMERA_MR_NORMAL , SCR_CAMERA_MR_BRIGHT };

	for (int i = 0; auto rtH : rendetTargetHandles_)
	{
		rtH = texLoader.MakeScreen(screenNames[i], wsize.w, wsize.h);
		i++;
	}

	cameraScreenH_ = texLoader.MakeScreen(SCR_CAMERA_VIEW_SCREEN, wsize.w, wsize.h);
	colorShrinkScreenH_		= texLoader.MakeScreen(SCR_COLOR_SHRINK,		wsize.w / 2, wsize.h);
	emmisionShrinkScreenH_	= texLoader.MakeScreen(SCR_EMMISION_SHRINK,	wsize.w / 2, wsize.h);

	modelEndrendering_ = make_shared<ModelEndRendering>();

	CreateRenderTargetHeap();
}

RendererManager::~RendererManager()
{
}

void RendererManager::Update()
{
	sceneInf_->Update();
	for (auto& meshRenderer : meshRenderers_)
	{
		meshRenderer.renderer->ComputeUpdate(meshRenderer.meshs_);
	}
	dx12_.GetCommand().Execute();
}

void RendererManager::Draw()
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 影のレンダリング
	std::list<int> nullTargets;
	texLoader.SetDrawScreen(nullTargets, TexLoader::DepthType::light);
	texLoader.ClsDrawScreen();

	dx12_.SetViewAndScissor(SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	for (auto& meshRenderer : meshRenderers_)
	{
		meshRenderer.renderer->DrawShadow(meshRenderer.meshs_);
	}

	// マルチレンダリング
	std::list<int> rtList;
	for (auto rtHandle : rendetTargetHandles_)
	{
		rtList.emplace_back(rtHandle);
	}

	texLoader.SetDrawScreen(rtList, TexLoader::DepthType::camera);
	texLoader.ClsDrawScreen();

	dx12_.SetDefaultViewAndScissor();

	for (auto& meshRenderer : meshRenderers_)
	{
		meshRenderer.renderer->Draw(meshRenderer.meshs_);
	}

	dx12_.DrawEfk();

	auto& spriteDrawer = dx12_.GetSpriteDrawer();

	// 縮小バッファへの描画
	auto drawToShrinkBuffer
		= [&spriteDrawer = spriteDrawer, &texLoader = texLoader, &rendetTargetHandles = rendetTargetHandles_]
			(const int shrinkHandle, const RenderTargetType type)
	{
		spriteDrawer.SetDrawScreen(shrinkHandle);

		texLoader.ClsDrawScreen();
		int highLumH = rendetTargetHandles[Uint64(type)];
		unsigned int w, h;
		int top, left;
		top = left = 0;
		texLoader.GetGraphSize(highLumH, w, h);
		for (int i = 0; i < SHRINK_CNT; ++i)
		{
			w /= 2;
			h /= 2;
			spriteDrawer.DrawExtendGraph(left, top, left + w, top + h, highLumH);
			top += h;
		}
	};

	// color縮小バッファへの描画
	drawToShrinkBuffer(colorShrinkScreenH_, RenderTargetType::color);

	// emmision縮小バッファへの描画
	drawToShrinkBuffer(emmisionShrinkScreenH_, RenderTargetType::bright);

	// レンダリング結果の合成
	spriteDrawer.SetDrawScreen(cameraScreenH_);
	texLoader.ClsDrawScreen();

	dx12_.SetDefaultViewAndScissor();

	spriteDrawer.SetMaterial(modelEndrendering_);
	spriteDrawer.DrawGraph(0, 0, rendetTargetHandles_.front());
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
}

void RendererManager::AddMesh(Mesh* mesh)
{
	auto meshTypeIndex = Uint64(mesh->GetMeshType());
	assert(meshTypeIndex < meshRenderers_.size());
	meshRenderers_[meshTypeIndex].meshs_.emplace_back(mesh);
}

void RendererManager::RemoveMesh(Mesh* mesh)
{
	auto meshTypeIndex = Uint64(mesh->GetMeshType());
	assert(meshTypeIndex < meshRenderers_.size());

	auto& vec = meshRenderers_[meshTypeIndex].meshs_;
	std::erase_if(vec, [mesh](Mesh* m) {return m == mesh; });
}

SceneInf& RendererManager::GetSceneInf()
{
	return *sceneInf_;
}

void RendererManager::DrawImGui()
{
	sceneInf_->DrawImGui();
}

void RendererManager::CreateRenderTargetHeap()
{
	auto& dev = dx12_.GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = Uint32(RenderTargetType::max);
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	H_ASSERT(dev.CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(renderTargetHeap_.ReleaseAndGetAddressOf())));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto& texLoader = dx12_.GetTexLoader();
	wstring multiRenderingNames[] = {SCR_CAMERA_MR_COLOR, SCR_CAMERA_MR_NORMAL, SCR_CAMERA_MR_BRIGHT};

	auto wsize = Application::Instance().GetWindowSize();
	auto incSize = dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	auto cpuHandle = renderTargetHeap_->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; auto& name : multiRenderingNames)
	{
		rendetTargetHandles_[i] = texLoader.MakeScreen(name, wsize.w, wsize.h);
		TextureResorce texRes;
		texLoader.GetTextureResouse(name, texRes);
		rtvDesc.Format = texRes.resource.buffer->GetDesc().Format;
		dev.CreateRenderTargetView(texRes.resource.buffer.Get(), &rtvDesc, cpuHandle);
		cpuHandle.ptr += incSize;
		i++;
	}
}