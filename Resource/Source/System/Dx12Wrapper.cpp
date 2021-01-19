#include "Dx12Wrapper.h"
#include "Application.h"
#include <DirectXTex.h>
#include "Command.h"
#include "TexLoader.h"
#include "2D/SpriteDrawer.h"
#include "SoundManager.h"
#include "3D/Camera.h"
#include "System/FileSystem.h"
#include "3D/RendererManager.h"
#include "Utility/Tool.h"
#include "Utility/dx12Tool.h"
#include "Utility/Input.h"
#include "Utility/SettingData.h"
#include "Utility/Cast.h"
#include "Utility/ImGuiTool.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#pragma comment(lib,"LLGId.lib")
#else
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"LLGI.lib")
#endif

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx12.h"

using namespace DirectX;
using namespace std;

Dx12Wrapper::Dx12Wrapper(HWND hwnd): hwnd_(hwnd)
{
}

Dx12Wrapper::~Dx12Wrapper()
{
	efkRenderer_->Release();
	efkManager_->Release();
	efkMemoryPool_->Release();
	efkCmdList_->Release();
	effect_->Release();
}

bool Dx12Wrapper::Init()
{
#ifdef _DEBUG
	// デバッグレイヤーの有効化
	ComPtr<ID3D12Debug> debuglayer;
	D3D12GetDebugInterface(IID_PPV_ARGS(debuglayer.ReleaseAndGetAddressOf()));
	debuglayer->EnableDebugLayer();
#endif

	// Deviceの作成
	H_ASSERT(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())));

	// DXGIFactoryの作成
	H_ASSERT(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf())));

	// コマンド系を作成
	cmd_ = make_shared<Command>(*dev_.Get());

	// スワップチェインの作成
	CreateSwapChain();

	texLoader_ = make_shared<TexLoader>(GetDevice(), GetCommand(), *swapChain_.Get());
	soundManager_ = make_shared<SoundManager>();

	spriteDrawer_ = make_shared<SpriteDrawer>(*this);

	texLoader_->SetDrawScreen(GetBackScreenHandle());
	rendererManager_ = std::make_shared<RendererManager>(*this);
	fileSystem_ = std::make_shared<FileSystem>();

	InitEfk();

	InitImGui();

	CreateSettingData();

	return true;
}

void Dx12Wrapper::InitEfk()
{
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer_ = EffekseerRendererDX12::Create(dev_.Get(), &cmd_->CommandQueue(), 2, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);
	efkManager_ = Effekseer::Manager::Create(8000);
	// 描画用インスタンスから描画機能を設定
	efkManager_->SetSpriteRenderer(efkRenderer_->CreateSpriteRenderer());
	efkManager_->SetRibbonRenderer(efkRenderer_->CreateRibbonRenderer());
	efkManager_->SetRingRenderer(efkRenderer_->CreateRingRenderer());
	efkManager_->SetTrackRenderer(efkRenderer_->CreateTrackRenderer());
	efkManager_->SetModelRenderer(efkRenderer_->CreateModelRenderer());
	efkManager_->SetTextureLoader(efkRenderer_->CreateTextureLoader());
	efkManager_->SetModelLoader(efkRenderer_->CreateModelLoader());
	efkManager_->SetMaterialLoader(efkRenderer_->CreateMaterialLoader());
	//メモリプール
	efkMemoryPool_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer_);
	//コマンドリスト作成
	efkCmdList_ = EffekseerRendererDX12::CreateCommandList(efkRenderer_, efkMemoryPool_);
	//コマンドリストセット
	efkRenderer_->SetCommandList(efkCmdList_);
	// エフェクトの読込
	effect_ = Effekseer::Effect::Create(efkManager_, u"Resource/Effect/Laser01.efk");
	assert(effect_ != nullptr);
	efkHandle_ = efkManager_->Play(
		effect_,
		Effekseer::Vector3D(0, 10, 0));
	efkManager_->SetRotation(efkHandle_, { 0, 1, 0 }, 180.0f * XM_PI / 180.0f);
}

void Dx12Wrapper::InitImGui()
{
	CreateDescriptorHeap(dev_.Get(), imguiHeap_);

	auto result = ImGui::CreateContext();
	assert(result != nullptr);

	bool bResult = ImGui_ImplWin32_Init(hwnd_);
	assert(bResult);

	bResult = ImGui_ImplDX12_Init(
		dev_.Get(), 
		3, 
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		imguiHeap_.Get(), 
		imguiHeap_->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap_->GetGPUDescriptorHandleForHeapStart());

	assert(bResult);
}

void Dx12Wrapper::CreateSettingData()
{
	CreateConstantBufferAndHeap(dev_.Get(), settingData_, 
		settingCB_, settingHeap_, sizeof(SettingData));
	*settingData_ = SettingData();

	settingData_->outlineColor = {0.0f, 1.0f, 0.0f};
	settingData_->emmisionColor = { 1.0f, 0.1f, 0.1f };
	settingData_->emmisionRate = 10.0f;
}

ID3D12Device& Dx12Wrapper::GetDevice()
{
	return *dev_.Get();
}

Command& Dx12Wrapper::GetCommand()
{
	return *cmd_;
}

RendererManager& Dx12Wrapper::GetRendererManager()
{
	return *rendererManager_;
}

FileSystem& Dx12Wrapper::GetFileSystem()
{
	return *fileSystem_;
}

int Dx12Wrapper::GetBackScreenHandle()
{
	return swapChain_->GetCurrentBackBufferIndex();
}

void Dx12Wrapper::ScreenFlip()
{
	texLoader_->ScreenFlip(*swapChain_.Get());
}

void Dx12Wrapper::DrawEfk()
{
	efkMemoryPool_->NewFrame();
	EffekseerRendererDX12::BeginCommandList(efkCmdList_, &cmd_->CommandList());
	efkRenderer_->SetCommandList(efkCmdList_);
	auto& camera = rendererManager_->GetCameraObject().GetCamera();

	auto XMView = camera->GetViewMatrix();
	auto XMProj = camera->GetProjMatrix();
	Effekseer::Matrix44 view, proj;

	for (int j = 0; j < 4; ++j) 
	{
		for (int i = 0; i < 4; ++i) 
		{
			view.Values[j][i] = XMView.r[j].m128_f32[i];
			proj.Values[j][i] = XMProj.r[j].m128_f32[i];
		}
	}

	efkManager_->Update();
	efkManager_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	if (Application::Instance().GetInput().GetButtonDown(DIK_SPACE))
	{
		if (efkManager_->Exists(efkHandle_))
		{
			efkManager_->StopEffect(efkHandle_);
		}
		else
		{
			efkHandle_ = efkManager_->Play(
				effect_,
				Effekseer::Vector3D(0, 10, 0));
			efkManager_->SetRotation(efkHandle_, { 0, 1, 0 }, 180.0f * XM_PI / 180.0f);
		}
	}
	// 投影行列を設定
	efkRenderer_->SetProjectionMatrix(proj);
	// カメラ行列を設定
	efkRenderer_->SetCameraMatrix(view);

	efkRenderer_->BeginRendering();
	efkManager_->Draw();
	efkRenderer_->EndRendering(); 

	efkRenderer_->SetCommandList(nullptr);
    EffekseerRendererDX12::EndCommandList(efkCmdList_);
}

void Dx12Wrapper::BeginDrawImGui()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Config");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);

	rendererManager_->ImGuiDraw();

	DrawImGuiForSetting();
}

void Dx12Wrapper::DrawImGuiForSetting()
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Setting"))
	{
		bool outline = settingData_->outline != 0;
		ImGui::Checkbox("Outline", &outline);
		settingData_->outline = Uint32(outline);
		if (outline)
		{
			PickColorXMFLOAT3("OutlineColor", settingData_->outlineColor);				
		}

		bool emmision = settingData_->emmision != 0;
		ImGui::Checkbox("Emmision", &emmision);
		settingData_->emmision = Uint32(emmision); 
		if (emmision)
		{
			PickColorXMFLOAT3("EmmisionColor", settingData_->emmisionColor);
			ImGui::SliderFloat("EmmisionPower", &settingData_->emmisionRate, 0.0f, 20.0f);
		}

		bool dissolve = settingData_->dissolve != 0;
		ImGui::Checkbox("Dissolve", &dissolve);
		settingData_->dissolve = Uint32(dissolve);

		ImGui::TreePop();
	}
}

void Dx12Wrapper::EndDrawImGui()
{
	ImGui::End();
	ImGui::Render();

	auto& cmdList = cmd_->CommandList();
	cmdList.SetDescriptorHeaps(1, imguiHeap_.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), &cmdList);
}

void Dx12Wrapper::CreateSwapChain()
{
	auto wsize = Application::Instance().GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = wsize.w;
	swapChainDesc.Height = wsize.h;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Stereo = false;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.BufferCount = 2;

	H_ASSERT(dxgiFactory_->CreateSwapChainForHwnd(&cmd_->CommandQueue(), hwnd_, &swapChainDesc,
		nullptr, nullptr, (IDXGISwapChain1**)swapChain_.ReleaseAndGetAddressOf()));
}

TexLoader& Dx12Wrapper::GetTexLoader()
{
	return *texLoader_;
}

SoundManager& Dx12Wrapper::GetSoundManager()
{
	return *soundManager_;
}

SpriteDrawer& Dx12Wrapper::GetSpriteDrawer()
{
	return *spriteDrawer_;
}

void Dx12Wrapper::SetDefaultViewAndScissor()
{
	unsigned int screenW, screenH;
	texLoader_->GetScreenSize(screenW, screenH);

	SetViewAndScissor(screenW, screenH);
}

void Dx12Wrapper::SetViewAndScissor(const UINT width, const UINT height)
{
	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorRect = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Float(width);
	viewport.Height = Float(height);
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = width;
	scissorRect.bottom = height;

	auto& commandList = cmd_->CommandList();
	commandList.RSSetViewports(1, &viewport);
	commandList.RSSetScissorRects(1, &scissorRect);
}

void Dx12Wrapper::SetSettingData(const UINT rootPramIndex)
{
	auto& cmdList = cmd_->CommandList();
	cmdList.SetDescriptorHeaps(1, settingHeap_.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(rootPramIndex, 
		settingHeap_->GetGPUDescriptorHandleForHeapStart());
}
