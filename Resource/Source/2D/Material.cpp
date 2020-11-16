#include "Material.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"

Material::Material(const std::wstring& shaderPaht):shaderPath_(shaderPaht)
{
	CreateEachDataBuffer();
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
	cmdList.SetDescriptorHeaps(1, textureIndexHeap_.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(5, textureIndexHeap_->GetGPUDescriptorHandleForHeapStart());

	cmdList.SetDescriptorHeaps(1, constantFloatHeap_.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(6, constantFloatHeap_->GetGPUDescriptorHandleForHeapStart());
}

void Material::CreateEachDataBuffer()
{
	AddEachData();

	auto& dev = Application::Instance().GetDx12().GetDevice();
	CreateStructuredBuffer(&dev, texIndexResource.buffer,		textureIndexHeap_,	
		textureIndexVec_, mappedTexIndex, false);
	CreateStructuredBuffer(&dev, constantFloatResource.buffer,	constantFloatHeap_,	
		constantFloatVec_, mappedConstantFloat, false);
}
