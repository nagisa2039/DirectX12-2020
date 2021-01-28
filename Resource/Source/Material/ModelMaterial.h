#pragma once
#include "Material.h"

/// <summary>
/// ���f���̃}�e���A��
/// </summary>
class ModelMaterial :
    public Material
{
public:
    /// <param name="materialBaseVec">�}�e���A����{���z��</param>
    /// <param name="addTexIdxVec">�ǉ��e�N�X�`���C���f�b�N�X�z��</param>
    /// <param name="constFloatVec">�萔�z��</param>
    ModelMaterial(
        const std::vector<MaterialBase>& materialBaseVec = std::vector<MaterialBase>(), 
        const std::vector<int>& addTexIdxVec = std::vector<int>(),
        const std::vector<float>& constFloatVec = std::vector<float>());
    ~ModelMaterial();
private:
};