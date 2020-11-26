#pragma once
#include "SkeletalMeshData.h"
#include <string>

class PMXData :
	public SkeletalMeshData
{
public:
	// (モデルファイルパス)
	PMXData(std::string modelPath);
	~PMXData();

private:
	std::vector<uint8_t> info_;
	// pmdモデルの読み込み
	bool LoadFromPMX(std::string modelPath);

	void LoadVertexIndex(FILE * fp);

	void LoadVertex(FILE * fp);

	void ReadTextBuf(std::wstring& wstrBuf, FILE * fp);

	// マテリアルの読み込み
	void LoadMaterial(FILE * fp, std::string &modelPath);

	// ボーンの読み込み
	void LoadBone(FILE * fp);
};

