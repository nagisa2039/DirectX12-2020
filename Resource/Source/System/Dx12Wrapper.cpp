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

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"d3dcompiler.lib")

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

	return true;
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
