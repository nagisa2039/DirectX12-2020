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

namespace
{
	constexpr uint32_t shadow_resolution = 2048;
}

Dx12Wrapper::Dx12Wrapper(HWND hwnd): _hwnd(hwnd)
{
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

	_texLoader = make_shared<TexLoader>(GetDevice(), GetCommand());
	_texLoader->CreateSwapChainBuffer(*_swapChain.Get());

	_drawer = make_shared<Drawer>(GetDevice(), GetCommand(), GetTexLoader());

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

int Dx12Wrapper::LoagGraph(const std::string& filePath)
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

bool Dx12Wrapper::CreateDepthBuffer()
{
	// 深度バッファの作成
	auto wsize = Application::Instance().GetWindowSize();

	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, wsize.w, wsize.h);
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// 重要らしい
	D3D12_CLEAR_VALUE depthClearValue = {};
	// 深さの最大値1にする
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	if (FAILED(_dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// シャドウマップ用
	depthResDesc.Width = shadow_resolution;
	depthResDesc.Height = shadow_resolution;
	if (FAILED(_dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_lightDepthBuffer.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// 深度ステンシルビューの作成
	if (!CreateDSVAndSRV())
	{
		return false;
	}

	return true;
}

bool Dx12Wrapper::CreateDSVAndSRV()
{
	// 深度バッファビューの作成
	// デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2;		// 0 は描画深度 1 はライト深度
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(_dev->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(_depthDSVHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	auto DSVhandle = _depthDSVHeap->GetCPUDescriptorHandleForHeapStart();

	// 描画用
	_dev->CreateDepthStencilView(
		_depthBuffer.buffer.Get(),
		&dsvDesc,
		DSVhandle);
	DSVhandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// シャドウマップ用
	_dev->CreateDepthStencilView(
		_lightDepthBuffer.buffer.Get(),
		&dsvDesc,
		DSVhandle);

	//  SR用デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2;		// 0 は描画深度 1 はライト深度
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	if (FAILED(_dev->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(_depthSRVHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// SR用のビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	auto SRVhandle = _depthSRVHeap->GetCPUDescriptorHandleForHeapStart();

	// 描画用
	_dev->CreateShaderResourceView(
		_depthBuffer.buffer.Get(),
		&srvDesc,
		SRVhandle);
	SRVhandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// シャドウマップ用
	_dev->CreateShaderResourceView(
		_lightDepthBuffer.buffer.Get(),
		&srvDesc,
		SRVhandle);

	return true;
}