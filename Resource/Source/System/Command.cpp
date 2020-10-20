#include "Command.h"
#include "d3dx12.h"
#include "Utility/dx12Tool.h"

Command::Command(ID3D12Device& dev): dev_(dev)
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
	H_ASSERT(dev_.CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(cmdQue_.ReleaseAndGetAddressOf())));

	// コマンドアロケータの作成
	H_ASSERT(dev_.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAlc_.ReleaseAndGetAddressOf())));

	// コマンドリストの作成
	H_ASSERT(dev_.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlc_.Get(), nullptr, IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf())));
	cmdList_->Close();

	// コマンドのリセット
	CommandReset();

	// フェンスの作成
	H_ASSERT(dev_.CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())));
}

ID3D12GraphicsCommandList& Command::CommandList()
{
	return *cmdList_.Get();
}

ID3D12CommandQueue& Command::CommandQueue()
{
	return *cmdQue_.Get();
}

bool Command::CommandReset()
{
	H_ASSERT(cmdAlc_->Reset());
	H_ASSERT(cmdList_->Reset(cmdAlc_.Get(), nullptr));
	return true;
}

void Command::WaitFence()
{
	if (fence_->GetCompletedValue() != fenceValue_)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

void Command::Execute()
{
	cmdList_->Close();

	ID3D12CommandList* cmds{ cmdList_.Get() };
	cmdQue_->ExecuteCommandLists(1, &cmds);
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);

	WaitFence();

	CommandReset();
}
