#pragma once
#include "Material.h"
class ModelMaterial :
    public Material
{
public:
    ModelMaterial(
        const std::vector<MaterialBase>& materialBaseVec, 
        const std::vector<int>& addTexIdxVec);
    ~ModelMaterial();
private:
};