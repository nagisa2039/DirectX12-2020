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
	cmdList.SetGraphicsRootDescriptorTable(5, texIndexResource_.handle);
	cmdList.SetGraphicsRootDescriptorTable(6, constFloatResource_.handle);
}

ComPtr<ID3D12PipelineState>& Material::GetPipelineState()
{
	return pipelineState_;
}

std::vector<MaterialBase>& Material::GetMaterialBaseVec()
{
	return materialBaseResource_.elements;
}

std::vector<int>& Material::GetAddTextureIndexVec()
{
	return texIndexResource_.elements;
}

std::vector<float>& Material::GetConstFloatVec()
{
	return constFloatResource_.elements;
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
	CreateSB(texIndexResource_);
	CreateSB(constFloatResource_);
}
