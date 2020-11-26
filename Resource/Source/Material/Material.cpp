#include "Material.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"

Material::Material(const std::wstring& shaderPaht):shaderPath_(shaderPaht)
{
}

Material::~Material()
{
}

const std::wstring& Material::GetShaderPath() const
{
	return shaderPath_;
}

void Material::SetEachDescriptorHeap(ID3D12GraphicsCommandList& cmdList)
{
	cmdList.SetDescriptorHeaps(1, heap_.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(4, materialBaseResource_.handle);
	cmdList.SetGraphicsRootDescriptorTable(5, addTexIndexResource_.handle);
	cmdList.SetGraphicsRootDescriptorTable(6, constFloatResource_.handle);
}

ComPtr<ID3D12PipelineState>& Material::GetPipelineState()
{
	return pipelineState_;
}

void Material::SetMaterialBase(const size_t index, const MaterialBase& value)
{
	assert(index < materialBaseResource_.elements.size());
	materialBaseResource_.elements[index] = value;
	materialBaseResource_.mapped[index] = value;
}

void Material::SetAddTexIndex(const size_t index, const int& value)
{
	assert(index < addTexIndexResource_.elements.size());
	addTexIndexResource_.elements[index] = value;
	addTexIndexResource_.mapped[index] = value;
}

void Material::SetConstFloat(const size_t index, const float& value)
{
	assert(index < constFloatResource_.elements.size());
	constFloatResource_.elements[index] = value;
	constFloatResource_.mapped[index] = value;
}

void Material::CreateEachDataBuffer()
{
	auto& dev = Application::Instance().GetDx12().GetDevice();

	CreateDescriptorHeap(&dev, heap_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3);

	auto GPUHandle = heap_->GetGPUDescriptorHandleForHeapStart();
	auto CPUHandle = heap_->GetCPUDescriptorHandleForHeapStart();
	auto stride = dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto CreateSB = 
		[&dev = dev, &GHandle = GPUHandle, &CHandle = CPUHandle, &stride = stride](auto& matResource)
	{
		if (matResource.elements.size() <= 0)
		{
			matResource.elements.resize(1);
		}
		CreateStructuredBuffer(&dev, matResource.resource.buffer, CHandle,
			matResource.elements, matResource.mapped, false);

		matResource.handle = GHandle;

		GHandle.ptr += stride;
		CHandle.ptr += stride;
	};
	
	CreateSB(materialBaseResource_);
	CreateSB(addTexIndexResource_);
	CreateSB(constFloatResource_);
}
