#include "Dx12Wrapper.h"
#include "Application.h"
#include <DirectXTex.h>
#include "Utility/Tool.h"
#include "Command.h"
#include "TexLoader.h"
#include "Utility/dx12Tool.h"
#include "2D/SpriteDrawer.h"
#include "SoundManager.h"
#include "3D/Camera.h"
#include "System/FileSystem.h"
#include "3D/RendererManager.h"
#include "Utility/Input.h"


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

using namespace DirectX;
using namespace std;

Dx12Wrapper::Dx12Wrapper(HWND hwnd): hwnd_(hwnd)
{
}

Dx12Wrapper::~Dx12Wrapper()
{
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

	//InitEfk();

	return true;
}

void Dx12Wrapper::InitEfk()
{
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer_ = EffekseerRendererDX12::Create(dev_.Get(), &cmd_->CommandQueue(), 2, &format, 1, DXGI_FORMAT_D32_FLOAT, false, 2000);
	efkManager_ = Effekseer::Manager::Create(2000);
	// 描画用インスタンスから描画機能を設定
	efkManager_->SetSpriteRenderer(efkRenderer_->CreateSpriteRenderer());
	efkManager_->SetRibbonRenderer(efkRenderer_->CreateRibbonRenderer());
	efkManager_->SetRingRenderer(efkRenderer_->CreateRingRenderer());
	efkManager_->SetTrackRenderer(efkRenderer_->CreateTrackRenderer());
	efkManager_->SetModelRenderer(efkRenderer_->CreateModelRenderer());
	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	efkManager_->SetTextureLoader(efkRenderer_->CreateTextureLoader());
	efkManager_->SetModelLoader(efkRenderer_->CreateModelLoader());
	//メモリプール
	efkMemoryPool_ = EffekseerRendererDX12::CreateSingleFrameMemoryPool(efkRenderer_);
	//コマンドリスト作成
	efkCmdList_ = EffekseerRendererDX12::CreateCommandList(efkRenderer_, efkMemoryPool_);
	//コマンドリストセット
	efkRenderer_->SetCommandList(efkCmdList_);
	// 投影行列を設定
	efkRenderer_->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(90.0f / 180.0f * 3.14f, 1280.f / 720.f, 1.0f,
			100.0f));
	// カメラ行列を設定
	efkRenderer_->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(::Effekseer::Vector3D(0.0f, 0.0f, -
			10.0f), ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
	// エフェクトの読込
	effect_ = Effekseer::Effect::Create(efkManager_, (const EFK_CHAR*)L"Resource/Laser01.efk");
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
	if (Application::Instance().GetInput().GetButtonDown(DIK_SPACE))
	{
		auto efkHandle = efkManager_->Play(effect_, Effekseer::Vector3D(0, 0, 0));
	}
	efkMemoryPool_->NewFrame();
	EffekseerRendererDX12::BeginCommandList(efkCmdList_, &cmd_->CommandList());
	efkRenderer_->BeginRendering();
	efkManager_->Draw();
	efkRenderer_->EndRendering();
    EffekseerRendererDX12::EndCommandList(efkCmdList_);
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
