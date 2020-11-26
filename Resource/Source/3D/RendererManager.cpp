#include "RendererManager.h"
#include "Model/SkeletalMeshRenderer.h"
#include "Primitive/PrimitiveRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "Utility/Constant.h"
#include "System/Application.h"
#include "3D/Camera.h"
#include "Utility/dx12Tool.h"
#include "2D/SpriteDrawer.h"
#include "Material/ModelEndRendering.h"
#include "Mesh.h"
#include "3D/Model/SkeletalMesh.h"
#include "3D/Primitive/PlaneMesh.h"
#include "3D/Model/VMDMotion.h"
#include "Actor.h"

using namespace std;
using namespace DirectX;

namespace
{
	constexpr unsigned int SHRINK_CNT = 4;
}

RendererManager::RendererManager(Dx12Wrapper& dx12) :dx12_(dx12)
{
	meshRenderers_.resize(Uint64(Mesh::Type::max));
	meshRenderers_[Uint64(Mesh::Type::static_mesh)].renderer	= make_shared<PrimitiveRenderer>(dx12_);
	meshRenderers_[Uint64(Mesh::Type::skeletal_mesh)].renderer	= make_shared<SkeletalMeshRenderer>(dx12_);

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

	actors_.reserve(10);
	auto& staticMeshRenderer = meshRenderers_[Uint64(Mesh::Type::static_mesh)];
	auto& skeletalMeshRenderer = meshRenderers_[Uint64(Mesh::Type::skeletal_mesh)];

	auto AddPlaneActor = [&](RendererManager::MeshRender& mr)
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<PlaneMesh>(actor, dx12, XMFLOAT3(0.0f, 0.0f, 0.0f), (1000.0f / 536.0f) * 80.0f, 80.0f, L"image/fiona.png");
		mr.meshs_.emplace_back(mesh);
		actor->AddComponent(mesh);
		actors_.emplace_back(actor);
	};

	auto AddMMDActor = [&](const std::string& modelFilePath, const std::string& motionFilePath, RendererManager::MeshRender& mr)
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<SkeletalMesh>(actor, modelFilePath, dx12_, GetVMDMotion(motionFilePath));
		mr.meshs_.emplace_back(mesh);
		actor->AddComponent(mesh);
		actors_.emplace_back(actor);
	};

	AddMMDActor("Resource/Model/桜ミク/雪ミク.pmd",					"Resource/VMD/swing2.vmd",			skeletalMeshRenderer);
	AddMMDActor("Resource/Model/桜ミク/mikuXS桜ミク.pmd",			"Resource/VMD/swing2.vmd",			skeletalMeshRenderer);
	AddMMDActor("Resource/Model/ぽんぷ長式神風/ぽんぷ長式神風.pmx", "Resource/VMD/swing2.vmd",			skeletalMeshRenderer);
	AddMMDActor("Resource/Model/ぽんぷ長式村雨/ぽんぷ長式村雨.pmx", "Resource/VMD/ヤゴコロダンス.vmd",  skeletalMeshRenderer);
	AddMMDActor("Resource/Model/葛城/葛城.pmd",						"Resource/VMD/ヤゴコロダンス.vmd",	skeletalMeshRenderer);
	for (int i = 0; auto& actor : actors_)
	{
		int moveZ = (i + 1) / 2;
		int moveX = moveZ * ((i % 2) * 2 - 1);

		auto trans = actor->GetTransform();
		trans.pos = XMFLOAT3(8.0f * moveX, 0.0f, 5.0f * moveZ);
		actor->SetTransform(trans);
		i++;
	}

	AddPlaneActor(staticMeshRenderer);
}

RendererManager::~RendererManager()
{
	actors_.clear();
}

void RendererManager::Update()
{
	dx12_.GetCamera().Update();

	for (auto& actor : actors_)
	{
		actor->Update();
	}
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

VMDMotion& RendererManager::GetVMDMotion(std::string motionPath)
{
	if (!vmdMotions_.contains(motionPath))
	{
		vmdMotions_.emplace(motionPath, make_shared<VMDMotion>(motionPath));
	}
	return *vmdMotions_[motionPath];
}