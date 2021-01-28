#pragma once
#include "SkeletalMeshData.h"

/// <summary>
/// PMD�f�[�^
/// </summary>
class PMDData :
	public SkeletalMeshData
{
public:
	// <param name="modelPath">(���f���t�@�C���p�X)</param>
	PMDData(std::wstring modelPath);
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

