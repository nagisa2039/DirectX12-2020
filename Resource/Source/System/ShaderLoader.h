#pragma once
#include <unordered_map>
#include <string>
#include "Utility/ComPtr.h"
#include <D3Dcommon.h>

/// <summary>
/// Shader�Ǘ��N���X
/// </summary>
class ShaderLoader
{
public:
	ShaderLoader();
	~ShaderLoader();

	/// <summary>
	/// Shader�̎擾
	/// </summary>
	/// <param name="shaderPath">�t�@�C���p�X</param>
	/// <param name="entoryPoint">�G���g���[�|�C���g</param>
	/// <param name="shaderModel">�V�F�[�_�[���f��</param>
	Microsoft::WRL::ComPtr<ID3DBlob> GetShader(const LPCWSTR& shaderPath, const LPCSTR& entoryPoint, const LPCSTR& shaderModel);

	/// <summary>
	/// �V�F�[�_�[���f���̎擾
	/// </summary>
	std::string GetShaderModel()const;

private:
	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> shaderMap_;
};

