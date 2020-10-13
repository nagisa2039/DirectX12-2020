#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <vector>
#include <memory>
#include "Command.h"
#include <array>
#include <DirectXMath.h>
#include <string>
#include "TextureStruct.h"

using Microsoft::WRL::ComPtr;

class Command;
class TexLoader;
class Drawer;

class Dx12Wrapper
{
public:
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	bool Init();

	ID3D12Device& GetDevice();
	Command& GetCommand();

	TexLoader& GetTexLoader();
	Drawer& GetDrawer();

	int LoagGraph(const std::string& filePath);
	void DrawGraph(const int x, const int y, const int handle);
	
	void ClsDrawScreen();
	void ScreenFlip();
	void DrawEnd();
	void SetDrawScreen(const int screenHandle);
	int GetBackScreenHandle();

	const DummyTextures& GetDummyTextures();
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);
	TextureResorce& GetTextureResouse(const int handle);


private:
	HWND _hwnd;

	ComPtr<ID3D12Device> _dev = nullptr;
	ComPtr<IDXGIFactory6> _dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> _swapChain = nullptr;

	std::shared_ptr<TexLoader> _texLoader;
	std::shared_ptr<Drawer> _drawer;
	std::shared_ptr<Command> _cmd;

	// 描画用深度バッファ
	Resource _depthBuffer;
	// シャドウマップ用深度バッファ
	Resource _lightDepthBuffer;

	ComPtr<ID3D12DescriptorHeap> _depthDSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _depthSRVHeap = nullptr;

	void CreateSwapChain();
	// 深度バッファの作成
	bool CreateDepthBuffer();
	// 深度ステンシルビューの作成
	bool CreateDSVAndSRV();
};

