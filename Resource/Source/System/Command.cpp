#include "Command.h"
#include "d3dx12.h"
#include "Utility/dx12Tool.h"

Command::Command(ID3D12Device& dev): _dev(dev)
{
	Init();
}

Command::~Command()
{
}

void Command::Init()
{
	// コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	H_ASSERT(_dev.CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(_cmdQue.ReleaseAndGetAddressOf())));

	// コマンドアロケータの作成
	H_ASSERT(_dev.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAlc.ReleaseAndGetAddressOf())));

	// コマンドリストの作成
	H_ASSERT(_dev.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlc.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf())));
	_cmdList->Close();

	// コマンドのリセット
	CommandReset();

	// フェンスの作成
	H_ASSERT(_dev.CreateFence(_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())));
}

ID3D12GraphicsCommandList& Command::CommandList()
{
	return *_cmdList.Get();
}

ID3D12CommandQueue& Command::CommandQueue()
{
	return *_cmdQue.Get();
}

bool Command::CommandReset()
{
	H_ASSERT(_cmdAlc->Reset());
	H_ASSERT(_cmdList->Reset(_cmdAlc.Get(), nullptr));
	return true;
}

void Command::WaitFence()
{
	if (_fence->GetCompletedValue() != _fenceValue)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceValue, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

void Command::Execute()
{
	_cmdList->Close();

	ID3D12CommandList* cmds{ _cmdList.Get() };
	_cmdQue->ExecuteCommandLists(1, &cmds);
	_cmdQue->Signal(_fence.Get(), ++_fenceValue);

	WaitFence();

	CommandReset();
}
