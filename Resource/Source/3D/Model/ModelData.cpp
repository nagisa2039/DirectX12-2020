#include "ModelData.h"
#include "Utility/Cast.h"

ModelData::ModelData()
{
}


ModelData::~ModelData()
{
}

std::vector<ModelData::Vertex>& ModelData::GetVertexData()
{
	return vertexData_;
}

std::vector<uint32_t>& ModelData::GetIndexData()
{
	return indexData_;
}

std::vector<ModelData::Material>& ModelData::GetMaterialData()
{
	return materials_;
}

std::vector<ModelData::Bone>& ModelData::GetBoneData()
{
	return bones_;
}

std::vector<ModelData::MultiTexturePath>& ModelData::GetTexturePaths()
{
	return texPaths_;
}