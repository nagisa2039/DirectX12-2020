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
	ID3D12Device& _dev;

	ComPtr<ID3D12CommandAllocator> _cmdAlc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;
	ComPtr<ID3D12CommandQueue> _cmdQue = nullptr;

	ComPtr<ID3D12Fence> _fence = nullptr;
	UINT64 _fenceValue = 0;

	void WaitFence();
	bool CommandReset();

};

