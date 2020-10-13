#include "Dx12Wrapper.h"
#include "Application.h"
#include <DirectXTex.h>
#include "Utility/Tool.h"
#include "Command.h"
#include "TexLoader.h"
#include "Utility/dx12Tool.h"
#include "Drawer.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;
using namespace std;

Dx12Wrapper::Dx12Wrapper(HWND hwnd): _hwnd(hwnd)
{
	_camera.eye = { 0, 20, -30 };
	_camera.target = { 0, 10, 0 };
	_camera.up = { 0, 1, 0 };
	_mappedCam = nullptr;
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
	H_ASSERT(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())));

	// DXGIFactoryの作成
	H_ASSERT(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf())));

	// コマンド系を作成
	_cmd = make_shared<Command>(*_dev.Get());

	// スワップチェインの作成
	CreateSwapChain();

	// カメラの作成
	CreateCameraConstantBufferAndView();

	_texLoader = make_shared<TexLoader>(GetDevice(), GetCommand());
	_texLoader->CreateSwapChainBuffer(*_swapChain.Get());

	_drawer = make_shared<Drawer>(GetDevice(), GetCommand(), GetTexLoader());

	UpdateSceneMatrix();

	return true;
}

ID3D12Device& Dx12Wrapper::GetDevice()
{
	return *_dev.Get();
}

Command& Dx12Wrapper::GetCommand()
{
	return *_cmd;
}

int Dx12Wrapper::LoadGraph(const std::string& filePath)
{
	return GetTexLoader().LoadTexture(filePath);
}

void Dx12Wrapper::DrawGraph(const int x, const int y, const int handle)
{
	_drawer->DrawGraph(x, y, handle);
}

void Dx12Wrapper::ClsDrawScreen()
{
	_texLoader->ClsDrawScreen();
}

void Dx12Wrapper::ScreenFlip()
{
	_texLoader->ScreenFlip(*_swapChain.Get());
}

void Dx12Wrapper::DrawEnd()
{
	_drawer->End();
}

void Dx12Wrapper::SetDrawScreen(const int screenHandle)
{
	_texLoader->SetDrawScreen(screenHandle);
}

int Dx12Wrapper::MakeScreen(const UINT width, const UINT height)
{
	return _texLoader->MakeScreen(width, height);
}

int Dx12Wrapper::GetBackScreenHandle()
{
	return _swapChain->GetCurrentBackBufferIndex();
}

const DummyTextures& Dx12Wrapper::GetDummyTextures()
{
	return _texLoader->GetDummyTextures();
}

bool Dx12Wrapper::GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes)
{
	return _texLoader->GetTextureResouse(texPath, texRes);
}

TextureResorce& Dx12Wrapper::GetTextureResouse(const int handle)
{
	return _texLoader->GetTextureResouse(handle);
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

	H_ASSERT(_dxgiFactory->CreateSwapChainForHwnd(&_cmd->CommandQueue(), _hwnd, &swapChainDesc,
		nullptr, nullptr, (IDXGISwapChain1**)_swapChain.ReleaseAndGetAddressOf()));
}

TexLoader& Dx12Wrapper::GetTexLoader()
{
	return *_texLoader;
}

Drawer& Dx12Wrapper::GetDrawer()
{
	return *_drawer;
}

void Dx12Wrapper::SetCameraDescriptorHeap(const UINT rootParamIdx)
{
	// カメラ用デスクリプタヒープの設定
	auto& commandList = _cmd->CommandList();
	commandList.SetDescriptorHeaps(1, _cameraHeap.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(rootParamIdx,
		_cameraHeap->GetGPUDescriptorHandleForHeapStart());
}

void Dx12Wrapper::SetDefaultViewAndScissor()
{
	_drawer->SetDefaultViewAndScissor();
}

bool Dx12Wrapper::CreateCameraConstantBufferAndView()
{
	CreateConstantBuffer(_dev.Get(), _cameraCB, sizeof(*_mappedCam));
	_cameraCB->Map(0, nullptr, (void**)&_mappedCam);

	CreateDescriptorHeap(_dev.Get(), _cameraHeap);

	// 定数バッファビューの作成
	CreateConstantBufferView(_dev.Get(), _cameraCB, _cameraHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Dx12Wrapper::UpdateSceneMatrix()
{
	// カメラの更新
	auto wsize = Application::Instance().GetWindowSize();
	XMVECTOR eyePos = XMLoadFloat3(&_camera.eye);
	XMVECTOR targetPos = XMLoadFloat3(&_camera.target);
	XMVECTOR upVec = XMLoadFloat3(&_camera.up);
	//XMVECTOR lightVec = XMLoadFloat3(&_mappedSetting->light_dir);

	auto view = XMMatrixLookAtLH(eyePos, targetPos, upVec);
	auto proj = XMMatrixPerspectiveFovLH(
		_camera.fov,
		static_cast<float>(wsize.w) / static_cast<float>(wsize.h),
		0.05f, 1000.0f);

	_mappedCam->view = view;
	_mappedCam->proj = proj;
	_mappedCam->eye = _camera.eye;
	_mappedCam->invProj = XMMatrixInverse(nullptr, proj);
}