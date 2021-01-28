#pragma once
#include "Material.h"

/// <summary>
/// 基本マテリアル
/// 追加情報が不必要なシェーダーに使用
/// </summary>
class StanderedMaterial :
    public Material
{
public:
	/// <param name="shaderPath">シェーダーパス</param>
	StanderedMaterial(const std::wstring& shaderPath);
	~StanderedMaterial();
};

