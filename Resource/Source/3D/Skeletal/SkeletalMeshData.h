#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "SkeletalMeshVertex.h"

class SkeletalMeshData
{
public :

	struct Material
	{
		DirectX::XMFLOAT4 diffuse;	// ディフューズ色
		DirectX::XMFLOAT3 specular;	// 鏡面反射光
		DirectX::XMFLOAT3 ambient;	// 環境構成分
		float power;	// スペキュラ乗数
		uint32_t indeicesNum;	// インデックス数
	};

	struct MultiTexturePath
	{
		std::wstring texPath;	// 通常テクスチャパス
		std::wstring sphPath;	// 乗算テクスチャパス
		std::wstring spaPath;	// 加算テクスチャパス
		std::wstring subPath;	// サブテクスチャ
		std::wstring toonPath;	// toonテクスチャパス
	};

	struct Bone
	{
		std::wstring name;
		int parentIdx = 0;
		DirectX::XMFLOAT3 startPos  = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの始点
		DirectX::XMFLOAT3 endPos	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの終点
	};

	SkeletalMeshData();
	~SkeletalMeshData();

	// 頂点情報の取得
	const std::vector<SkeletalMeshVertex>& GetVertexData();
	// インデックス情報の取得
	const std::vector<uint32_t>& GetIndexData();
	// テクスチャパス情報の取得
	const std::vector<MultiTexturePath>& GetTexturePaths();
	// マテリアル情報の取得
	const std::vector<Material>& GetMaterialData();
	// ボーン情報の取得
	const std::vector<Bone>& GetBoneData();

protected:

	std::vector<SkeletalMeshVertex> vertexData_;	// 頂点データ
	std::vector<uint32_t> indexData_;	// 頂点インデックスデータ
	std::vector<MultiTexturePath> texPaths_;		//テクスチャのパス(相対)
	std::vector<Material> materials_;		// マテリアルデータ
	std::vector<Bone> bones_;
};

