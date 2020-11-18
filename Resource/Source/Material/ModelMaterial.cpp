#include "ModelMaterial.h"

ModelMaterial::ModelMaterial(
	const std::vector<MaterialBase>& materialBaseVec, 
	const std::vector<int>& addTexIdxVec)
	: Material(L"")
{
	materialBaseResource_.elements	= materialBaseVec;
	texIndexResource_.elements		= addTexIdxVec;

	CreateEachDataBuffer();
}

ModelMaterial::~ModelMaterial()
{
}
