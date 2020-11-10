#pragma once
#include <unordered_map>
#include <string>
#include "Utility/ComPtr.h"
#include <D3Dcommon.h>

/// <summary>
/// Shader管理クラス
/// </summary>
class ShaderLoader
{
public:
	ShaderLoader();
	~ShaderLoader();

	/// <summary>
	/// Shaderの取得
	/// </summary>
	/// <param name="shaderPath">ファイルパス</param>
	/// <param name="entoryPoint">エントリーポイント</param>
	/// <param name="shaderModel">シェーダーモデル</param>
	Microsoft::WRL::ComPtr<ID3DBlob> GetShader(const LPCWSTR& shaderPath, const LPCSTR& entoryPoint, const LPCSTR& shaderModel);

	/// <summary>
	/// シェーダーモデルの取得
	/// </summary>
	std::string GetShaderModel()const;

private:
	std::unordered_map<std::wstring, ComPtr<ID3DBlob>> shaderMap_;
};

