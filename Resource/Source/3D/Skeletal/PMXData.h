#pragma once
#include "SkeletalMeshData.h"
#include <string>

/// <summary>
/// PMX�f�[�^
/// </summary>
class PMXData :
	public SkeletalMeshData
{
public:
	// <param name="modelPath">(���f���t�@�C���p�X)</param>
	PMXData(std::wstring modelPath);
	~PMXData();

private:
	std::vector<uint8_t> info_;
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

