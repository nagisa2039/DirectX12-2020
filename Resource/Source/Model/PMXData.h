#pragma once
#include "ModelData.h"
#include <string>

class PMXData :
	public ModelData
{
public:
	// (���f���t�@�C���p�X)
	PMXData(std::string modelPath);
	~PMXData();

private:
	std::vector<uint8_t> _info;
	// pmd���f���̓ǂݍ���
	bool LoadFromPMX(std::string modelPath);

	void LoadVertexIndex(FILE * fp);

	void LoadVertex(FILE * fp);

	void ReadTextBuf(std::wstring& wstrBuf, FILE * fp);

	// �}�e���A���̓ǂݍ���
	void LoadMaterial(FILE * fp, std::string &modelPath);

	// �{�[���̓ǂݍ���
	void LoadBone(FILE * fp);
};

