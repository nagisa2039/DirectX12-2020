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

struct SettingData;

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

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEfk();

	/// <summary>
	/// 開始時のImGuiの描画
	/// </summary>
	void BeginDrawImGui();

	/// <summary>
	/// 終了時ImGuiの描画
	/// </summary>
	void EndDrawImGui();

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

	/// <summary>
	/// 設定データをセットする
	/// </summary>
	/// <param name="rootPramIndex">ルートシグネチャインデックス</param>
	void SetSettingData(const UINT rootPramIndex);

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

	EffekseerRenderer::Renderer* efkRenderer_ = nullptr;
	Effekseer::Manager* efkManager_ = nullptr;
	EffekseerRenderer::SingleFrameMemoryPool* efkMemoryPool_ = nullptr;
	EffekseerRenderer::CommandList* efkCmdList_ = nullptr;
	Effekseer::Effect* effect_ = nullptr;
	Effekseer::Handle efkHandle_ = {};
	int efkSoundH_;

	ComPtr<ID3D12DescriptorHeap> imguiHeap_;

	ComPtr<ID3D12DescriptorHeap> settingHeap_;
	ComPtr<ID3D12Resource> settingCB_;
	SettingData* settingData_ = nullptr;

	void CreateSwapChain();

	void InitEfk();

	void InitImGui();

	void CreateSettingData();

	void DrawImGuiForSetting();
};

