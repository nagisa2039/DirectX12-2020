#include "ModelMaterial.h"

ModelMaterial::ModelMaterial(
	const std::vector<MaterialBase>& materialBaseVec, 
	const std::vector<int>& addTexIdxVec, 
	const std::vector<float>& constFloatVec)
	: Material(L"")
{
	materialBaseResource_.elements	= materialBaseVec;
	addTexIndexResource_.elements		= addTexIdxVec;
	constFloatResource_.elements = constFloatVec;

	CreateEachDataBuffer();
}

ModelMaterial::~ModelMaterial()
{
}
