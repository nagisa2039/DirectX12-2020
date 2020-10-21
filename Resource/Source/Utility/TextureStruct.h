#pragma once
#include <DirectXTex.h>
#include "Utility/DirectXStruct.h"

struct DummyTextures
{
	Resource whiteTex;
	Resource blackTex;
	Resource gradTex;
};

struct TextureResorce
{
	DirectX::Image imageInf;
	Resource resource;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleForTex;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleForTex;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleForRtv;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleForRtv;
};