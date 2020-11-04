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

std::vector<MaterialIndex>& ModelData::GetMaterialIndexData()
{
	return materialIndexData_;
}

std::vector<ModelData::Bone>& ModelData::GetBoneData()
{
	return bones_;
}

void ModelData::SetVertexMaterialIndex()
{
	const auto indexNum = indexData_.size();
	const int primitiveVertNum = 3;
	assert(indexNum % primitiveVertNum == 0);
	materialIndexData_.resize(indexNum/ primitiveVertNum);

	int indexOffset = 0;
	for (decltype(MaterialIndex::index) matIdx = 0; auto& mat : materials_)
	{
		uint32_t index = 0;
		while (true)
		{
			materialIndexData_[(Uint64(indexOffset) + index) / primitiveVertNum].index = matIdx;
			index += primitiveVertNum;
			if (index >= mat.indeicesNum)
			{
				indexOffset += mat.indeicesNum;
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