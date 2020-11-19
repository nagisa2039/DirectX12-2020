#pragma once
#include "Material.h"
class ModelMaterial :
    public Material
{
public:
    ModelMaterial(
        const std::vector<MaterialBase>& materialBaseVec = std::vector<MaterialBase>(), 
        const std::vector<int>& addTexIdxVec = std::vector<int>(),
        const std::vector<float>& constFloatVec = std::vector<float>());
    ~ModelMaterial();
private:
};