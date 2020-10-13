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
	struct Camera
	{
		DirectX::XMFLOAT3 eye;
		DirectX::XMFLOAT3 target;
		DirectX::XMFLOAT3 up;
		float fov = DirectX::XM_PIDIV4;
	};

	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	bool Init();

	ID3D12Device& GetDevice();
	Command& GetCommand();

	TexLoader& GetTexLoader();
	Drawer& GetDrawer();

	int LoadGraph(const std::string& filePath);
	void DrawGraph(const int x, const int y, const int handle);
	
	void ClsDrawScreen();
	void ScreenFlip();
	void DrawEnd();
	void SetDrawScreen(const int screenHandle);
	int MakeScreen(const UINT width, const UINT height);
	int GetBackScreenHandle();

	const DummyTextures& GetDummyTextures();
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);
	TextureResorce& GetTextureResouse(const int handle);

	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	void SetDefaultViewAndScissor();

private:
	HWND _hwnd;

	ComPtr<ID3D12Device> _dev = nullptr;
	ComPtr<IDXGIFactory6> _dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain4> _swapChain = nullptr;

	std::shared_ptr<TexLoader> _texLoader;
	std::shared_ptr<Drawer> _drawer;
	std::shared_ptr<Command> _cmd;

	// カメラ行列用定数バッファ
	ComPtr<ID3D12Resource> _cameraCB = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> _cameraHeap = nullptr;
	
	// GPUに渡す行列をまとめた構造体
	struct Scene
	{
		//DirectX::XMMATRIX world;	// 合成前ワールド
		DirectX::XMMATRIX view;	//
		DirectX::XMMATRIX proj;	//
		DirectX::XMMATRIX invProj;	// プロジェクションの逆行列
		DirectX::XMMATRIX lightCamera;	//ライドから見たビュープロジェクション
		DirectX::XMMATRIX shadow;
		DirectX::XMFLOAT3 eye;	// 視点
	};
	// 定数バッファのアドレスを格納	
	// _cameraCBの内容を変更したいときはこいつを通じて変更してね
	Scene* _mappedCam;

	// 視点(カメラの位置)
	// 注視点(見る対象の位置)
	// 上ベクトル(上)
	Camera _camera;

	// カメラのバッファとビューを作成
	bool CreateCameraConstantBufferAndView();

	void CreateSwapChain();

	void UpdateSceneMatrix();
};

