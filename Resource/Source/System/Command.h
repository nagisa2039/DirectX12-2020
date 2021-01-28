#pragma once
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

/// <summary>
/// Command�n�Ǘ��N���X
/// </summary>
class Command
{
public:
	/// <param name="dev">�f�o�C�X�̎Q��</param>
	Command(ID3D12Device& dev);
	~Command();

	/// <summary>
	/// CommandList�̎擾
	/// </summary>
	ID3D12GraphicsCommandList& CommandList();

	/// <summary>
	/// CommandQueue�̎擾
	/// </summary>
	ID3D12CommandQueue& CommandQueue();

	/// <summary>
	/// �R�}���h�̎��s
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
	/// ������
	/// </summary>
	void Init();

	void WaitFence();
	bool CommandReset();

};

