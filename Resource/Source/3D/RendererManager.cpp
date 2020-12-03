#include "RendererManager.h"
#include "Skeletal/SkeletalMeshRenderer.h"
#include "Static/StaticMeshRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "Utility/Constant.h"
#include "System/Application.h"
#include "3D/Camera.h"
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
	camera_ = make_shared<CameraObject>(dx12.GetCommand(), dx12.GetDevice());

	meshRenderers_.resize(Uint64(Mesh::Type::max));
	meshRenderers_[Uint64(Mesh::Type::static_mesh)].renderer	= make_shared<StaticMeshRenderer>(dx12_, camera_->GetCamera());
	meshRenderers_[Uint64(Mesh::Type::skeletal_mesh)].renderer	= make_shared<SkeletalMeshRenderer>(dx12_, camera_->GetCamera());

	auto wsize = Application::Instance().GetWindowSize();
	auto& texLoader = dx12_.GetTexLoader();

	wstring screenNames[] = { D3D_CAMERA_MR_COLOR, D3D_CAMERA_MR_NORMAL , D3D_CAMERA_MR_BRIGHT };

	for (int i = 0; auto rtH : rendetTargetHandles_)
	{
		rtH = texLoader.MakeScreen(screenNames[i], wsize.w, wsize.h);
		i++;
	}

	cameraScreenH_ = texLoader.MakeScreen(D3D_CAMERA_VIEW_SCREEN, wsize.w, wsize.h);
	shrinkScreenH_ = texLoader.MakeScreen(D3D_CAMERA_SHRINK_SCREEN, wsize.w / 2, wsize.h);

	modelEndrendering_ = make_shared<ModelEndRendering>();

	CreateRenderTargetHeap();
}

RendererManager::~RendererManager()
{
}

void RendererManager::Update()
{
	camera_->Update();
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

	// 縮小バッファへの描画
	auto& spriteDrawer = dx12_.GetSpriteDrawer();
	spriteDrawer.SetDrawScreen(shrinkScreenH_);
	
	texLoader.ClsDrawScreen(); 
	int highLumH = rendetTargetHandles_[Uint64(RenderTargetType::bright)];
	unsigned int w, h;
	int top,left;
	top = left = 0;
	texLoader.GetGraphSize(highLumH, w, h);
	for (int i = 0; i < SHRINK_CNT; ++i)
	{
		w /= 2;
		h /= 2;
		spriteDrawer.DrawExtendGraph(left, top, left + w, top + h, highLumH);
		top += h;
	}

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
	wstring multiRenderingNames[] = {D3D_CAMERA_MR_COLOR, D3D_CAMERA_MR_NORMAL, D3D_CAMERA_MR_BRIGHT};

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