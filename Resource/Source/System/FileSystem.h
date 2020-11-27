#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

class SkeletalMeshData;
class VMDMotion;

/// <summary>
/// �t�@�C���̓ǂݍ��݃N���X
/// </summary>
class FileSystem
{
public:
	FileSystem();
	~FileSystem()=default;

	/// <summary>
	/// SkeletalMeshData�̎擾
	/// �ǂݍ��ݍς݂łȂ���Γǂݍ��݂��s��
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	SkeletalMeshData& GetSkeletalMeshData(const std::wstring& path);

	/// <summary>
	/// VMDMotion�̎擾
	/// �ǂݍ��ݍς݂łȂ���Γǂݍ��݂��s��
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	VMDMotion& GetVMDMotion(const std::wstring& path);

private:
	std::unordered_map<std::wstring, std::shared_ptr<SkeletalMeshData>> skeletalMeshDatas_;
	std::unordered_map<std::wstring, std::shared_ptr<VMDMotion>> vmdMotions_;

	using skeletalLoadFunc_t = std::function<std::shared_ptr<SkeletalMeshData>(const std::wstring)>;
	std::unordered_map<std::wstring, skeletalLoadFunc_t> skeletalLoadFuncs_;
};

