#pragma once
#include <d3d12.h>
#include "Utility/ComPtr.h"

class Command;

// DirectXリソース
struct Resource
{
	// バッファ
	ComPtr<ID3D12Resource> buffer;
	// 状態
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	/// <summary>
	/// バッファにバリア(状態変更)をする
	/// </summary>
	/// <param name="cmd">コマンド管理クラス</param>
	/// <param name="changeState">変更する状態</param>
	/// <returns>成否</returns>
	bool Barrier(Command& cmd, const D3D12_RESOURCE_STATES changeState);
};

/// <summary>
/// ヒープにバインド済みのリソース
/// </summary>
struct ResourceBindHeap
{
	// リソース
	Resource resource = {};
	// CPUヒープハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuH = {};
	// GPUヒープハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE gpuH = {};
};