#pragma once
#include "Utility/ComPtr.h"
#include <d3d12.h>

class DescriptorHeap
{
public:
	DescriptorHeap(ID3D12Device& dev, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const UINT numDescriptors);
	~DescriptorHeap();

	struct Handle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu;
	};
	Handle AddCBV(ComPtr<ID3D12Resource>& buffer);
	Handle AddStructuredBuffer(ComPtr<ID3D12Resource>& buffer, const UINT elementNum);

	Handle GetNextHandle()const;

	void Set(ID3D12GraphicsCommandList& cmdList)const;

private:
	ID3D12Device& dev_;
	ComPtr<ID3D12DescriptorHeap> heap_;
	UINT numDescriptor_;
};

