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
	cmdList.SetDescriptorHeaps(1, materialBaseResource_.heap.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(5, materialBaseResource_.heap->GetGPUDescriptorHandleForHeapStart());

	cmdList.SetDescriptorHeaps(1, texIndexResource_.heap.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(6, texIndexResource_.heap->GetGPUDescriptorHandleForHeapStart());

	cmdList.SetDescriptorHeaps(1, constFloatResource_.heap.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(7, constFloatResource_.heap->GetGPUDescriptorHandleForHeapStart());
}

ComPtr<ID3D12PipelineState>& Material::GetPipelineState()
{
	return pipelineState_;
}

MaterialBase& Material::GetMaterialBase()
{
	return materialBaseResource_.elements[0];
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

	auto CreateSB = [&dev = dev](auto& matResource) 
	{
		if (matResource.elements.size() <= 0)
		{
			matResource.elements.resize(1);
		}
		CreateStructuredBuffer(&dev, matResource.resource.buffer, matResource.heap,
			matResource.elements, matResource.mapped, false);
	};
	
	CreateSB(materialBaseResource_);
	CreateSB(texIndexResource_);
	CreateSB(constFloatResource_);
}
