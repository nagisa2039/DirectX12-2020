#pragma once
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

/// <summary>
/// Command系管理クラス
/// </summary>
class Command
{
public:
	/// <param name="dev">デバイスの参照</param>
	Command(ID3D12Device& dev);
	~Command();

	/// <summary>
	/// CommandListの取得
	/// </summary>
	ID3D12GraphicsCommandList& CommandList();

	/// <summary>
	/// CommandQueueの取得
	/// </summary>
	ID3D12CommandQueue& CommandQueue();

	/// <summary>
	/// コマンドの実行
	/// </summary>
	void Execute();

private:
	ID3D12Device& dev_;

	ComPtr<ID3D12CommandAllocator> cmdAlc_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList_ = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQue_ = nullptr;

	ComPtr<ID3D12Fence> fence_ = nullptr;
	UINT64 fenceValue_ = 0;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	void WaitFence();
	bool CommandReset();

};

