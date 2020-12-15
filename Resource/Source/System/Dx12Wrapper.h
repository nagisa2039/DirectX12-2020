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
#include "System/FileSystem.h"

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

using Microsoft::WRL::ComPtr;

class Command;
class TexLoader;
class SpriteDrawer;
class SoundManager;
class RendererManager;
class FileSystem;

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
	/// サウンド管理蔵種の取得
	/// </summary>
	SoundManager& GetSoundManager();

	/// <summary>
	/// 2D描画管理クラスの取得
	/// </summary>
	SpriteDrawer& GetSpriteDrawer();

	/// <summary>
	/// 3D描画管理クラスの取得
	/// </summary>
	RendererManager& GetRendererManager();

	/// <summary>
	/// ファイル読み込み管理クラスの取得
	/// </summary>
	FileSystem& GetFileSystem();
	
	/// <summary>
	/// 現在の裏画面ハンドルの取得
	/// </summary>
	int GetBackScreenHandle();

	/// <summary>
	/// 裏画面と表画面の入れ替え
	/// </summary>
	void ScreenFlip();

	void DrawEfk();

	/// <summary>
	/// 全画面にViewportとScissorを設定する
	/// </summary>
	void SetDefaultViewAndScissor();

	/// <summary>
	/// 指定範囲ににViewportとScissorを設定する
	/// </summary>
	/// <param name="width">画面幅</param>
	/// <param name="height">画面高さ</param>
	void SetViewAndScissor(const UINT width, const UINT height);

private:
	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<IDXGIFactory6> dxgiFactory_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::shared_ptr<TexLoader> texLoader_;
	std::shared_ptr<SpriteDrawer> spriteDrawer_;
	std::shared_ptr<Command> cmd_;
	std::shared_ptr<SoundManager> soundManager_;
	std::shared_ptr<RendererManager> rendererManager_;
	std::shared_ptr<FileSystem> fileSystem_;

	EffekseerRenderer::Renderer* efkRenderer_;
	Effekseer::Manager* efkManager_;
	EffekseerRenderer::SingleFrameMemoryPool* efkMemoryPool_;
	EffekseerRenderer::CommandList* efkCmdList_;
	Effekseer::Effect* effect_;

	void CreateSwapChain();

	void InitEfk();
};

