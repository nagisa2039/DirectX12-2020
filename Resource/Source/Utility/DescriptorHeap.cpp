#include "DescriptorHeap.h"
#include "Utility/Cast.h"
#include "dx12Tool.h"

DescriptorHeap::DescriptorHeap(
	ID3D12Device& dev, 
	D3D12_DESCRIPTOR_HEAP_TYPE heapType, 
	UINT numDescriptors)
	: dev_(dev)
{
	CreateDescriptorHeap(&dev, heap_, heapType, numDescriptors);
	numDescriptor_ = 0;
}

DescriptorHeap::~DescriptorHeap()
{
}

DescriptorHeap::Handle DescriptorHeap::AddCBV(ComPtr<ID3D12Resource>& buffer)
{
	auto heapType = heap_->GetDesc().Type;
	assert(heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto handle = GetNextHandle();

	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
	viewDesc.BufferLocation = buffer->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(buffer->GetDesc().Width);
	dev_.CreateConstantBufferView(&viewDesc, handle.cpu);
	numDescriptor_++;
	return handle;
}

DescriptorHeap::Handle DescriptorHeap::AddStructuredBuffer(ComPtr<ID3D12Resource>& buffer, const UINT elementNum)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	auto handle = GetNextHandle();
	auto bufSize = buffer->GetDesc().Width;
	viewDesc.Buffer.NumElements = elementNum;
	viewDesc.Buffer.StructureByteStride = bufSize / elementNum;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	viewDesc.Format = buffer->GetDesc().Format;
	dev_.CreateShaderResourceView(buffer.Get(), &viewDesc, handle.cpu);
	numDescriptor_++;
	return handle;
}

DescriptorHeap::Handle DescriptorHeap::GetNextHandle() const
{
	auto cHandle = heap_->GetCPUDescriptorHandleForHeapStart();
	auto gHandle = heap_->GetGPUDescriptorHandleForHeapStart();
	auto strideSize = dev_.GetDescriptorHandleIncrementSize(heap_->GetDesc().Type);
	cHandle.ptr += Uint64(strideSize) * numDescriptor_;
	gHandle.ptr += Uint64(strideSize) * numDescriptor_;
	return Handle{ cHandle, gHandle };
}

void DescriptorHeap::Set(ID3D12GraphicsCommandList& cmdList) const
{
	cmdList.SetDescriptorHeaps(1, heap_.GetAddressOf());
}