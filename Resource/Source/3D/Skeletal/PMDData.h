#pragma once
#include "SkeletalMeshData.h"

/// <summary>
/// PMDデータ
/// </summary>
class PMDData :
	public SkeletalMeshData
{
public:
	// <param name="modelPath">(モデルファイルパス)</param>
	PMDData(std::wstring modelPath);
	~PMDData();

private:
	std::vector<int> toonIndexVec_;

	// pmdモデルの読み込み
	bool LoadFromPMD(std::string modelPath);

	void LoadVertexIndex(FILE * fp);

	void LoadVertex(FILE * fp);

	// マテリアルの読み込み
	void LoadMaterial(FILE * fp, std::string &modelPath);

	// ボーンの読み込み
	void LoadBone(FILE * fp);
};

