#include "ShaderLoader.h"
#include <d3dcompiler.h>
#include "Utility/Tool.h"
#include "Utility/dx12Tool.h"
#include <d3d12.h>
#include <sstream>

ShaderLoader::ShaderLoader()
{
}

ShaderLoader::~ShaderLoader()
{
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderLoader::GetShader(const LPCWSTR& shaderPath, const LPCSTR& entoryPoint, const LPCSTR& shaderModel)
{
	ComPtr<ID3DBlob> shaderBlob = nullptr;

//	auto filePath = WStringFromString(GetFilePath(StringFromWString(shaderPath)));
//	filePath = filePath.substr(0, filePath.rfind(L"."));
//	std::wstringstream ss;
//#ifdef _DEBUG
//	ss << L"x64/Debug/" << filePath << L".cso";
//#else
//	ss << L"Resource/Shader" << filePath;
//#endif
//
//	auto ret = D3DReadFileToBlob(ss.str().c_str(), shaderBlob.ReleaseAndGetAddressOf());
//	H_ASSERT(ret);
//	shaderMap_[ss.str()] = shaderBlob;

	std::wstringstream ss;
	ss << shaderPath << L"/" << WStringFromString(entoryPoint);
	if (shaderMap_.contains(ss.str()))
	{
		return shaderMap_[ss.str()];
	}
	ComPtr<ID3DBlob> erBlob = nullptr;

	if (FAILED(D3DCompileFromFile(shaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entoryPoint, shaderModel,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, shaderBlob.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	{
		OutputBolbString(erBlob.Get());
		assert(false);
		return shaderBlob;
	}
	
	shaderMap_[ss.str()] = shaderBlob;

	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderLoader::GetPixelShader(const LPCWSTR& shaderPath)
{
	std::stringstream ss;
	ss << "ps_" << GetShaderModel();
	return GetShader(shaderPath, "PS", ss.str().c_str());
}

std::string ShaderLoader::GetShaderModel() const
{
	return "5_1";
}
