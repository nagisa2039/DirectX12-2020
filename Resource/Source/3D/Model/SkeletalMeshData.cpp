#include "SkeletalMeshData.h"
#include "Utility/Cast.h"

SkeletalMeshData::SkeletalMeshData()
{
}


SkeletalMeshData::~SkeletalMeshData()
{
}

std::vector<SkeletalMeshData::Vertex>& SkeletalMeshData::GetVertexData()
{
	return vertexData_;
}

std::vector<uint32_t>& SkeletalMeshData::GetIndexData()
{
	return indexData_;
}

std::vector<SkeletalMeshData::Material>& SkeletalMeshData::GetMaterialData()
{
	return materials_;
}

std::vector<SkeletalMeshData::Bone>& SkeletalMeshData::GetBoneData()
{
	return bones_;
}

std::vector<SkeletalMeshData::MultiTexturePath>& SkeletalMeshData::GetTexturePaths()
{
	return texPaths_;
}