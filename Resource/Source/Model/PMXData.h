#pragma once
#include "ModelData.h"
#include <string>

class PMXData :
	public ModelData
{
public:
	// (モデルファイルパス)
	PMXData(std::string modelPath);
	~PMXData();

private:
	std::vector<uint8_t> _info;
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

