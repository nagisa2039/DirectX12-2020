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
#include "Utility/TextureStruct.h"

using Microsoft::WRL::ComPtr;

class Command;
class TexLoader;
class SpriteDrawer;

class Dx12Wrapper
{
public:
	/// <summary>
	/// DirectX管理クラス
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	bool Init();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	ID3D12Device& GetDevice();

	/// <summary>
	/// コマンドの取得
	/// </summary>
	Command& GetCommand();

	/// <summary>
	/// テクスチャリソースの管理クラス取得
	/// </summary>
	TexLoader& GetTexLoader();

	/// <summary>
	/// 2D描画管理クラスの取得
	/// </summary>
	SpriteDrawer& GetSpriteDrawer();
	
	/// <summary>
	/// 現在の裏画面ハンドルの取得
	/// </summary>
	int GetBackScreenHandle();

	/// <summary>
	/// 裏画面と表画面の入れ替え
	/// </summary>
	void ScreenFlip();

	/// <summary>
	/// カメラのヒープを設定
	/// </summary>
	/// <param name="rootParamIdx">rootSignatureインデックス</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// 全画面にViewportとScissorを設定する
	/// </summary>
	void SetDefaultViewAndScissor();

private:
	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<IDXGIFactory6> dxgiFactory_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::shared_ptr<TexLoader> texLoader_;
	std::shared_ptr<SpriteDrawer> spriteDrawer_;
	std::shared_ptr<Command> cmd_;

	// カメラ行列用定数バッファ
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;
	
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

	struct Camera
	{
		DirectX::XMFLOAT3 eye;
		DirectX::XMFLOAT3 target;
		DirectX::XMFLOAT3 up;
		float fov = DirectX::XM_PIDIV4;
	};

	// 定数バッファのアドレスを格納	
	// _cameraCBの内容を変更したいときはこいつを通じて変更してね
	Scene* mappedCam_;

	// 視点(カメラの位置)
	// 注視点(見る対象の位置)
	// 上ベクトル(上)
	Camera camera_;

	// カメラのバッファとビューを作成
	bool CreateCameraConstantBufferAndView();

	void CreateSwapChain();

	void UpdateSceneMatrix();
};

