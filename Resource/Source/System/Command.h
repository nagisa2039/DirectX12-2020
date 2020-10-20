#pragma once
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class Command
{
public:
	Command(ID3D12Device& dev);
	~Command();

	void Init();

	ID3D12GraphicsCommandList& CommandList();
	ID3D12CommandQueue& CommandQueue();

	void Execute();

private:
	ID3D12Device& dev_;

	ComPtr<ID3D12CommandAllocator> cmdAlc_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> cmdList_ = nullptr;
	ComPtr<ID3D12CommandQueue> cmdQue_ = nullptr;

	ComPtr<ID3D12Fence> fence_ = nullptr;
	UINT64 fenceValue_ = 0;

	void WaitFence();
	bool CommandReset();

};

