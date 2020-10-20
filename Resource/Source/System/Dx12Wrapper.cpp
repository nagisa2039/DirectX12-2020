#include "Dx12Wrapper.h"
#include "Application.h"
#include <DirectXTex.h>
#include "Utility/Tool.h"
#include "Command.h"
#include "TexLoader.h"
#include "Utility/dx12Tool.h"
#include "SpriteDrawer.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace std;

Dx12Wrapper::Dx12Wrapper(HWND hwnd): hwnd_(hwnd)
{
	camera_.eye = { 0, 20, -30 };
	camera_.target = { 0, 10, 0 };
	camera_.up = { 0, 1, 0 };
	mappedCam_ = nullptr;

	viewport_ = {};
	scissorRect_ = {};
}


Dx12Wrapper::~Dx12Wrapper()
{
}

bool Dx12Wrapper::Init()
{
	// デバッグレイヤーの有効化
	ComPtr<ID3D12Debug> debuglayer;
	D3D12GetDebugInterface(IID_PPV_ARGS(debuglayer.ReleaseAndGetAddressOf()));
	debuglayer->EnableDebugLayer();

	// Deviceの作成
	H_ASSERT(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())));

	// DXGIFactoryの作成
	H_ASSERT(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf())));

	// コマンド系を作成
	cmd_ = make_shared<Command>(*dev_.Get());

	// スワップチェインの作成
	CreateSwapChain();

	// カメラの作成
	CreateCameraConstantBufferAndView();

	texLoader_ = make_shared<TexLoader>(GetDevice(), GetCommand(), *swapChain_.Get());

	spriteDrawer_ = make_shared<SpriteDrawer>(*this);

	UpdateSceneMatrix();

	texLoader_->SetDrawScreen(GetBackScreenHandle());

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

SpriteDrawer& Dx12Wrapper::GetSpriteDrawer()
{
	return *spriteDrawer_;
}

void Dx12Wrapper::SetCameraDescriptorHeap(const UINT rootParamIdx)
{
	// カメラ用デスクリプタヒープの設定
	auto& commandList = cmd_->CommandList();
	commandList.SetDescriptorHeaps(1, cameraHeap_.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(rootParamIdx,
		cameraHeap_->GetGPUDescriptorHandleForHeapStart());
}

void Dx12Wrapper::SetDefaultViewAndScissor()
{
	unsigned int screenW, screenH;
	texLoader_->GetScreenSize(screenW, screenH);

	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.Width = Float(screenW);
	viewport_.Height = Float(screenH);
	viewport_.MaxDepth = 1.0f;
	viewport_.MinDepth = 0.0f;

	scissorRect_.left = 0;
	scissorRect_.top = 0;
	scissorRect_.right = screenW;
	scissorRect_.bottom = screenH;

	auto& commandList = cmd_->CommandList();
	commandList.RSSetViewports(1, &viewport_);
	commandList.RSSetScissorRects(1, &scissorRect_);
}

bool Dx12Wrapper::CreateCameraConstantBufferAndView()
{
	CreateConstantBuffer(dev_.Get(), cameraCB_, sizeof(*mappedCam_));
	cameraCB_->Map(0, nullptr, (void**)&mappedCam_);

	CreateDescriptorHeap(dev_.Get(), cameraHeap_);

	// 定数バッファビューの作成
	CreateConstantBufferView(dev_.Get(), cameraCB_, cameraHeap_->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Dx12Wrapper::UpdateSceneMatrix()
{
	// カメラの更新
	auto wsize = Application::Instance().GetWindowSize();
	XMVECTOR eyePos = XMLoadFloat3(&camera_.eye);
	XMVECTOR targetPos = XMLoadFloat3(&camera_.target);
	XMVECTOR upVec = XMLoadFloat3(&camera_.up);
	//XMVECTOR lightVec = XMLoadFloat3(&_mappedSetting->light_dir);

	auto view = XMMatrixLookAtLH(eyePos, targetPos, upVec);
	auto proj = XMMatrixPerspectiveFovLH(
		camera_.fov,
		static_cast<float>(wsize.w) / static_cast<float>(wsize.h),
		0.05f, 1000.0f);

	mappedCam_->view = view;
	mappedCam_->proj = proj;
	mappedCam_->eye = camera_.eye;
	mappedCam_->invProj = XMMatrixInverse(nullptr, proj);
}