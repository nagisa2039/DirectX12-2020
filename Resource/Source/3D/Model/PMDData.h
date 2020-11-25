#pragma once
#include "ModelData.h"
class PMDData :
	public ModelData
{
public:
	// (���f���t�@�C���p�X)
	PMDData(std::string modelPath);
	~PMDData();

private:
	std::vector<int> toonIndexVec_;

	// pmd���f���̓ǂݍ���
	bool LoadFromPMD(std::string modelPath);

	void LoadVertexIndex(FILE * fp);

	void LoadVertex(FILE * fp);

	// �}�e���A���̓ǂݍ���
	void LoadMaterial(FILE * fp, std::string &modelPath);

	// �{�[���̓ǂݍ���
	void LoadBone(FILE * fp);
};

