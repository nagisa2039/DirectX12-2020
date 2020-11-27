#include "FileSystem.h"
#include "3D/Skeletal/VMDMotion.h"
#include "Utility/Tool.h"
#include "3D/Skeletal/PMDData.h"
#include "3D/Skeletal/PMXData.h"

using namespace std;

FileSystem::FileSystem()
{
	skeletalLoadFuncs_[L"pmx"] = [](const std::wstring& path) 
	{
		return make_shared<PMXData>(path);
	};

	skeletalLoadFuncs_[L"pmd"] = [](const std::wstring& path)
	{
		return make_shared<PMDData>(path);
	};
}

SkeletalMeshData& FileSystem::GetSkeletalMeshData(const std::wstring& path)
{
	if (!skeletalMeshDatas_.contains(path))
	{
		auto ext = GetExtension(path);
		assert(skeletalLoadFuncs_.contains(ext));
		skeletalMeshDatas_[path] = skeletalLoadFuncs_[ext](path);
	}
	return *skeletalMeshDatas_[path];
}

VMDMotion& FileSystem::GetVMDMotion(const std::wstring& path)
{
	if (!vmdMotions_.contains(path))
	{
		vmdMotions_.emplace(path, make_shared<VMDMotion>(path));
	}
	return *vmdMotions_[path];
}
