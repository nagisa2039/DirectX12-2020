#include "ModelData.h"


ModelData::ModelData()
{
}


ModelData::~ModelData()
{
}

std::vector<ModelData::Vertex>& ModelData::GetVertexData()
{
	return _vertexData;
}

std::vector<uint32_t>& ModelData::GetIndexData()
{
	return _indexData;
}

std::vector<ModelData::Material>& ModelData::GetMaterialData()
{
	return _materials;
}

std::vector<ModelData::Bone>& ModelData::GetBoneData()
{
	return _bones;
}

std::vector<ModelData::MultiTexturePath>& ModelData::GetTexturePaths()
{
	return _texPaths;
}