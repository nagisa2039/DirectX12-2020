#pragma once
#include <DirectXTex.h>
#include "Utility/DirectXStruct.h"

// 仮テクスチャハンドル
struct DummyTextures
{
	// 白
	int whiteTexH;
	// 黒
	int blackTexH;
	// 上から下へのグラデーション
	int gradTexH;
};

// テクスチャリソース
struct TextureResorce
{
	// 画像情報
	DirectX::Image imageInf = {};
	// リソース
	Resource resource;
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleForTex = {};
	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleForRtv = {};
};