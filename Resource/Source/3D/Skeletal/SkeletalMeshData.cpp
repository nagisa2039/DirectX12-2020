#include "SkeletalMeshData.h"
#include "Utility/Cast.h"

SkeletalMeshData::SkeletalMeshData()
{
}


SkeletalMeshData::~SkeletalMeshData()
{
}

const std::vector<SkeletalMeshVertex>& SkeletalMeshData::GetVertexData()
{
	return vertexData_;
}

const std::vector<uint32_t>& SkeletalMeshData::GetIndexData()
{
	return indexData_;
}

const std::vector<SkeletalMeshData::Material>& SkeletalMeshData::GetMaterialData()
{
	return materials_;
}

const std::vector<SkeletalMeshData::Bone>& SkeletalMeshData::GetBoneData()
{
	return bones_;
}

const std::vector<SkeletalMeshData::MultiTexturePath>& SkeletalMeshData::GetTexturePaths()
{
	return texPaths_;
}