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

std::vector<uint16_t>& ModelData::GetMaterialIndexData()
{
	return materialIndexData_;
}

std::vector<ModelData::Bone>& ModelData::GetBoneData()
{
	return bones_;
}

void ModelData::SetVertexMaterialIndex()
{
	materialIndexData_.resize(indexData_.size());
	int indexOffset = 0;
	for (int matIdx = 0; auto& mat : materials_)
	{
		int index = 0;
		while (true)
		{
			materialIndexData_[Uint64(indexOffset) + index] = matIdx;
			if (++index >= mat.indeicesNum)
			{
				indexOffset += mat.indeicesNum;
				index = 0;
				matIdx++;
				break;
			}
		}
	}
}

std::vector<ModelData::MultiTexturePath>& ModelData::GetTexturePaths()
{
	return _texPaths;
}