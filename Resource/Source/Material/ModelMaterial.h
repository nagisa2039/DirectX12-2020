#pragma once
#include "Material.h"

/// <summary>
/// モデルのマテリアル
/// </summary>
class ModelMaterial :
    public Material
{
public:
    /// <param name="materialBaseVec">マテリアル基本情報配列</param>
    /// <param name="addTexIdxVec">追加テクスチャインデックス配列</param>
    /// <param name="constFloatVec">定数配列</param>
    ModelMaterial(
        const std::vector<MaterialBase>& materialBaseVec = std::vector<MaterialBase>(), 
        const std::vector<int>& addTexIdxVec = std::vector<int>(),
        const std::vector<float>& constFloatVec = std::vector<float>());
    ~ModelMaterial();
private:
};