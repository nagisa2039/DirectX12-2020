#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Shader/Struct/SkeletalMeshVertex.h"

/// <summary>
/// ボーン有モデルデータ
/// </summary>
class SkeletalMeshData
{
public :
	// マテリアル
	struct Material
	{
		DirectX::XMFLOAT4 diffuse;	// ディフューズ色
		DirectX::XMFLOAT3 specular;	// 鏡面反射光
		DirectX::XMFLOAT3 ambient;	// 環境構成分
		float power;	// スペキュラ乗数
		uint32_t indeicesNum;	// インデックス数
	};

	// マテリアルごとのテクスチャパス
	struct MultiTexturePath
	{
		std::wstring texPath;	// 通常テクスチャパス
		std::wstring sphPath;	// 乗算テクスチャパス
		std::wstring spaPath;	// 加算テクスチャパス
		std::wstring subPath;	// サブテクスチャ
		std::wstring toonPath;	// toonテクスチャパス
	};

	// ボーン情報
	struct Bone
	{
		std::wstring name;
		int parentIdx = 0;
		DirectX::XMFLOAT3 startPos  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの始点
		DirectX::XMFLOAT3 endPos	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの終点
	};

	SkeletalMeshData();
	~SkeletalMeshData();

	/// <summary>
	/// 頂点情報の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<SkeletalMeshVertex>& GetVertexData();

	/// <summary>
	/// インデックス情報の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<uint32_t>& GetIndexData();

	/// <summary>
	/// テクスチャパス情報の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<MultiTexturePath>& GetTexturePaths();

	/// <summary>
	/// マテリアル情報の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<Material>& GetMaterialData();

	/// <summary>
	/// ボーン情報の取得
	/// </summary>
	/// <returns></returns>
	const std::vector<Bone>& GetBoneData();

protected:

	std::vector<SkeletalMeshVertex> vertexData_;	// 頂点データ
	std::vector<uint32_t> indexData_;	// 頂点インデックスデータ
	std::vector<MultiTexturePath> texPaths_;		//テクスチャのパス(相対)
	std::vector<Material> materials_;		// マテリアルデータ
	std::vector<Bone> bones_;
};

