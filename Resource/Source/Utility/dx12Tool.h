#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include "Tool.h"

#define H_ASSERT(X) assert(SUCCEEDED(X));

namespace
{
	void CreateConstantBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& pBuffer, UINT resourceSize)
	{
		auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resWidth = AlignmentValue(resourceSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resWidth);

		H_ASSERT(dev->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(pBuffer.ReleaseAndGetAddressOf())));
	}

	void CreateDescriptorHeap(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pHeap, UINT numDescriptors = 1)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		H_ASSERT(dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(pHeap.ReleaseAndGetAddressOf())));
	}

	void CreateConstantBufferView(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
		viewDesc.BufferLocation = buff->GetGPUVirtualAddress();
		viewDesc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);
		dev->CreateConstantBufferView(&viewDesc, handle);
	}
}