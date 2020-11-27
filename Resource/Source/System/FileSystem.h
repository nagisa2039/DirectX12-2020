#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

class SkeletalMeshData;
class VMDMotion;

/// <summary>
/// ファイルの読み込みクラス
/// </summary>
class FileSystem
{
public:
	FileSystem();
	~FileSystem()=default;

	/// <summary>
	/// SkeletalMeshDataの取得
	/// 読み込み済みでなければ読み込みを行う
	/// </summary>
	/// <param name="path">ファイルパス</param>
	SkeletalMeshData& GetSkeletalMeshData(const std::wstring& path);

	/// <summary>
	/// VMDMotionの取得
	/// 読み込み済みでなければ読み込みを行う
	/// </summary>
	/// <param name="path">ファイルパス</param>
	VMDMotion& GetVMDMotion(const std::wstring& path);

private:
	std::unordered_map<std::wstring, std::shared_ptr<SkeletalMeshData>> skeletalMeshDatas_;
	std::unordered_map<std::wstring, std::shared_ptr<VMDMotion>> vmdMotions_;

	using skeletalLoadFunc_t = std::function<std::shared_ptr<SkeletalMeshData>(const std::wstring)>;
	std::unordered_map<std::wstring, skeletalLoadFunc_t> skeletalLoadFuncs_;
};

