#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DirectXTex.h>

class Command;
using Microsoft::WRL::ComPtr;

struct Resource
{
	ComPtr<ID3D12Resource> buffer;
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	bool Barrier(Command& cmd, const D3D12_RESOURCE_STATES changeState);
};

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