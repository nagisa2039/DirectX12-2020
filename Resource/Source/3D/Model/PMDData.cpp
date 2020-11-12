#include "PMDData.h"
#include "Utility/Tool.h"
#include <sstream>	// ������X�g���[���p
#include <iomanip>	// ������}�j�s�����[�^�p(n�����낦��n���߂Ȃ�)


using namespace std;
using namespace DirectX;

PMDData::PMDData(std::string modelPath)
{
	LoadFromPMD(modelPath);
}


PMDData::~PMDData()
{
}

bool PMDData::LoadFromPMD(std::string modelPath)
{
	FILE* fp = nullptr;
	fopen_s(&fp, modelPath.c_str(), "rb");

	assert(fp != nullptr);
	if (fp == nullptr)return false;


	// �w�b�_�̓ǂݍ���
	// #pragma pack(1) �A���C�����g���P�o�C�g�ɂ���
#pragma pack(1)
	struct PMDHeader
	{
		char signature[3];	// 3
		// �p�f�B���O1������
		float version;		// 4
		char model_name[20];//	20
		char comment[256];//256
	};//283�o�C�g
#pragma pack()

	PMDHeader pmdheader;
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	// ���_
	LoadVertex(fp);

	// ���_�C���f�b�N�X
	LoadVertexIndex(fp);

	// �}�e���A���̓ǂݍ���
	LoadMaterial(fp, modelPath);

	SetVertexMaterialIndex();

	// �{�[���̓ǂݍ���
	LoadBone(fp);

	fclose(fp);

	return true;
}
void PMDData::LoadVertexIndex(FILE * fp)
{
	int indexNum = 0;
	fread(&indexNum, sizeof(indexNum), 1, fp);

	indexData_.resize(indexNum);
	for (int idx = 0; idx < indexNum; idx++)
	{
		fread(&indexData_[idx], sizeof(uint16_t), 1, fp);
	}
}
void PMDData::LoadVertex(FILE * fp)
{
	// ���_
#pragma pack(1)
	struct t_Vertex
	{
		XMFLOAT3 pos;//12
		XMFLOAT3 normal_vec;//12
		XMFLOAT2 uv;//8
		uint16_t bone_num[2];//4
		uint8_t bone_weight;//1
		uint8_t edge_flag;	//1
	};//38�o�C�g
#pragma pack()

	// ���_
	int vertNum = 0;
	fread_s(&vertNum, sizeof(vertNum), sizeof(uint32_t), 1, fp);

	vector<t_Vertex> t_VertexVec(vertNum);
	fread(t_VertexVec.data(), sizeof(t_VertexVec[0]) * t_VertexVec.size(), 1, fp);

	vertexData_.resize(vertNum);
	for (int idx = 0; idx < vertNum; idx++)
	{
		vertexData_[idx].pos = t_VertexVec[idx].pos;
		vertexData_[idx].normal = t_VertexVec[idx].normal_vec;
		vertexData_[idx].uv = t_VertexVec[idx].uv;
		vertexData_[idx].boneIdx.x = t_VertexVec[idx].bone_num[0];
		vertexData_[idx].boneIdx.y = t_VertexVec[idx].bone_num[1];
		vertexData_[idx].weight.x = t_VertexVec[idx].bone_weight/100.0f;
		vertexData_[idx].weight.y = 1.0f - vertexData_[idx].weight.x;
	}
}
void PMDData::LoadMaterial(FILE * fp, std::string &modelPath)
{
	// �}�e���A��
#pragma pack(1)
	struct t_Material
	{
		XMFLOAT4 diffuse_color; // dr, dg, db, da
		float specularity;//�X�y�L�����搔
		XMFLOAT3 specular_color; // sr, sg, sb // ����F
		XMFLOAT3 mirror_color; // mr, mg, mb // ���F(ambient)
		uint8_t toon_index; // toon??.bmp //
		uint8_t edge_flag; // �֊s�A�e
						   //�p�f�B���O2���\�z�����c�����܂ł�46�o�C�g
		uint32_t face_vert_count; // �ʒ��_��
		char texture_file_name[20]; // �e�N�X�`���t�@�C����
	};
#pragma pack()

	int materialNum = 0;
	fread(&materialNum, sizeof(materialNum), 1, fp);

	vector<t_Material> materials;
	materials.resize(materialNum);

	fread(materials.data(), sizeof(materials[0]) * materialNum, 1, fp);

	materials_.resize(materialNum);
	texPaths_.resize(materialNum);
	int idx = 0;
	for (auto& material : materials_)
	{
		material.diffuse = materials[idx].diffuse_color;
		material.specular = materials[idx].specular_color;
		material.ambient = materials[idx].mirror_color;
		material.power = materials[idx].specularity;
		material.indeicesNum = materials[idx].face_vert_count;

		string texName = materials[idx].texture_file_name;
		// ���f������̑��΃p�X��proj����̑��΃p�X�ɕϊ�����
		if (texName != "")
		{
			auto nameVec = SplitFileName(texName);
			for (auto name : nameVec)
			{
				auto ext = GetExtension(name);
				if (ext == "sph")
				{
					texPaths_[idx].sphPath = WStringFromString(GetFolderPath(modelPath + name));
				}
				else if (ext == "spa")
				{
					texPaths_[idx].spaPath = WStringFromString(GetFolderPath(modelPath) + name);
				}
				else
				{
					texPaths_[idx].texPath = WStringFromString(GetFolderPath(modelPath) + name);
				}
			}
		}

		if (materials[idx].toon_index < 10)
		{
			ostringstream oss;
			oss << "toon/toon" << setw(2) << setfill('0') << static_cast<int>(materials[idx].toon_index + 1) << ".bmp";
			texPaths_[idx].toonPath = WStringFromString(oss.str());
		}

		idx++;
	}
}

void PMDData::LoadBone(FILE * fp)
{
#pragma pack(1)
	struct t_bone	// 39�o�C�g
	{
		char bone_name[20]; // �{�[����
		uint16_t parent_bone_index; // �e�{�[���ԍ�(�Ȃ��ꍇ��0xFFFF)
		uint16_t tail_pos_bone_index; // tail�ʒu�̃{�[���ԍ�(�`�F�[�����[�̏ꍇ��0xFFFF 0 ���⑫2) // �e�F�q��1�F���Ȃ̂ŁA��Ɉʒu���ߗp
		uint8_t bone_type; // �{�[���̎��
		uint16_t ik_parent_bone_index; // IK�{�[���ԍ�(�e��IK�{�[���B�Ȃ��ꍇ��0)
		XMFLOAT3 bone_head_pos; // x, y, z // �{�[���̃w�b�h
	};
#pragma pack()

	uint16_t boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	vector<t_bone> t_bones(boneNum);

	fread(t_bones.data(), sizeof(t_bones[0]) * boneNum, 1, fp);
	bones_.resize(boneNum);

	for (int idx = 0; idx < boneNum; idx++)
	{
		auto& bone = t_bones[idx];
		bones_[idx].name = WStringFromString(string(bone.bone_name));
		bones_[idx].parentIdx = bone.parent_bone_index;
		bones_[idx].startPos = bone.bone_head_pos;
		bones_[idx].endPos = t_bones[bone.tail_pos_bone_index].bone_head_pos;
	}
}