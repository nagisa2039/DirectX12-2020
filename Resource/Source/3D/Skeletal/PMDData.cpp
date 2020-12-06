#include "PMDData.h"
#include "Utility/Tool.h"
#include <sstream>	// ������X�g���[���p
#include <iomanip>	// ������}�j�s�����[�^�p(n�����낦��n���߂Ȃ�)


using namespace std;
using namespace DirectX;

PMDData::PMDData(std::wstring modelPath)
{
	LoadFromPMD(StringFromWString(modelPath));
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

	// �{�[���̓ǂݍ���
	LoadBone(fp);

	WORD ik_data_cnt;
	fread_s(&ik_data_cnt, sizeof(ik_data_cnt), sizeof(ik_data_cnt), 1, fp);

	//IIK---------------------------------
#pragma pack(1)
	struct IK_Data_t
	{
		WORD bone_idx;
		WORD target_bone_index;
		BYTE chain_length;
		WORD iterations;
		float control_weight;
	};

#pragma pack()
	struct IK_Data
	{
		WORD bone_idx			= 0;
		WORD target_bone_index	= 0;
		WORD iterations			= 0;
		float control_weight	= 0.0f;
		std::vector<WORD> child_bone_index;
	};

	std::vector<IK_Data> ik_datas(ik_data_cnt);
	for (auto & ik_data : ik_datas)
	{
		IK_Data_t d;
		fread_s(&d, sizeof(IK_Data_t), sizeof(IK_Data_t), 1, fp);

		IK_Data ikData{ d.bone_idx, d.target_bone_index, d.iterations, d.control_weight };
		ikData.child_bone_index.resize(d.chain_length);
		fread_s(ikData.child_bone_index.data(), sizeof(WORD) * ikData.child_bone_index.size(),
			sizeof(WORD), ikData.child_bone_index.size(), fp);
		ik_data = ikData;
	}

	//�\��X�g---------------------------------
	WORD skin_cnt;
	fread_s(&skin_cnt, sizeof(skin_cnt), sizeof(skin_cnt), 1, fp);

#pragma pack(1)
	struct Skin_Data_t
	{
		char skin_name[20]; //�@�\�
		DWORD skin_vert_count; // �\��p�̒��_��
		BYTE skin_type; // �\��̎�� // 0�Fbase�A1�F�܂�A2�F�ځA3�F���b�v�A4�F���̑�
	};

	struct Skin_Vert_Data
	{	// base  : �\��p�̒��_�̔ԍ�(���_���X�g�ɂ���ԍ�)
		// other : �\��p�̒��_�̔ԍ�(base�̔ԍ��Bskin_vert_index)
		DWORD skin_vert_index; 
		// base  : x, y, z // �\��p�̒��_�̍��W(���_���̂̍��W)
		// other : // x, y, z // �\��p�̒��_�̍��W�I�t�Z�b�g�l(base�ɑ΂���I�t�Z�b�g)
		float skin_vert_pos[3]; 
	};
#pragma pack()

	struct SkinData
	{
		BYTE skin_type = 0; // �\��̎�� // 0�Fbase�A1�F�܂�A2�F�ځA3�F���b�v�A4�F���̑�
		std::vector<Skin_Vert_Data> vertData;
	};

	vector<SkinData> skinDatas(skin_cnt);
	for (auto& skinData : skinDatas)
	{
		Skin_Data_t d;
		fread_s(&d, sizeof(Skin_Data_t), sizeof(Skin_Data_t), 1, fp);

		skinData.skin_type = d.skin_type;
		skinData.vertData.resize(d.skin_vert_count);
		fread_s(skinData.vertData.data(), sizeof(Skin_Vert_Data) * d.skin_vert_count, 
			sizeof(Skin_Vert_Data), d.skin_vert_count, fp);
	}

	//�\��g�p�\�����X�g---------------------------------
	BYTE skin_disp_count;
	fread_s(&skin_disp_count, sizeof(skin_disp_count), sizeof(skin_disp_count), 1, fp);
	vector<WORD> skin_index(skin_disp_count); // �\��ԍ�
	fread_s(skin_index.data(), sizeof(WORD) * skin_disp_count, sizeof(WORD), skin_disp_count, fp);

	//�{�[���g�p�g�����X�g---------------------------------
	BYTE bone_disp_name_count; // �{�[���g�p�̘g���� // �Z���^�[(1�ԏ�ɕ\�������g)�͊܂܂Ȃ�
	fread_s(&bone_disp_name_count, sizeof(bone_disp_name_count), sizeof(bone_disp_name_count), 1, fp);
	vector<char[50]> dispName(bone_disp_name_count);
	fread_s(dispName.data(), sizeof(char) * 50 * dispName.size(), sizeof(char) * 50, dispName.size(), fp);

	//�{�[���g�p�\�����X�g---------------------------------
	DWORD bone_disp_count; // �{�[���g�ɕ\������{�[���� (�g0(�Z���^�[)�������A���ׂẴ{�[���g�̍��v)
	fread_s(&bone_disp_count, sizeof(bone_disp_count), sizeof(bone_disp_count), 1, fp);

#pragma pack(1)
	struct Bone_Disp_t
	{
		WORD bone_index; // �g�p�{�[���ԍ�
		BYTE bone_disp_frame_index;
	};
#pragma pack()
	vector<Bone_Disp_t> bone_disps(bone_disp_count); // �g�p�{�[���f�[�^ (3Bytes/bone)
	fread_s(bone_disps.data(), sizeof(Bone_Disp_t) * bone_disps.size(), 
		sizeof(Bone_Disp_t), bone_disps.size(), fp);

	//�g��-----------------------------------------------------------------------------------

	//�p�ꖼ�Ή�---------------------------------
	BYTE english_name_compatibility;
	fread_s(&english_name_compatibility, sizeof(english_name_compatibility), 
		sizeof(english_name_compatibility), 1, fp);
	if (english_name_compatibility == 1)
	{
		// ���f����
		fseek(fp, 20, SEEK_CUR);
		// �R�����g
		fseek(fp, 256, SEEK_CUR);

		// �{�[����
		fseek(fp, Cast(long, 20 * bones_.size()), SEEK_CUR);

		// �\��X�g
		fseek(fp, Cast(long, 20 * (skinDatas.size()-1)), SEEK_CUR);

		// �{�[���g�p�g�����X�g
		fseek(fp, Cast(long, 50 * dispName.size()), SEEK_CUR);
	}

	//toon---------------------------------
	std::vector<char[100]> toonTexPath(10);
	fread_s(toonTexPath.data(), sizeof(char) * 100 * toonTexPath.size(), sizeof(char) * 100 * toonTexPath.size(), 1, fp);

	for (int i = 0; auto& path : texPaths_)
	{
		path.toonPath = WStringFromString(GetFolderPath(modelPath) + toonTexPath[toonIndexVec_[i]]);
	}

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

	auto XMFloat4FromXMFloat3 = [](const XMFLOAT3& f3, const float w)
	{
		return XMFLOAT4(f3.x, f3.y, f3.z, w);
	};

	for (int idx = 0; idx < vertNum; idx++)
	{
		vertexData_[idx].pos		= XMFloat4FromXMFloat3(t_VertexVec[idx].pos, 1.0f);
		vertexData_[idx].normal		= XMFloat4FromXMFloat3(t_VertexVec[idx].normal_vec, 0.0f);
		vertexData_[idx].uv			= t_VertexVec[idx].uv;
		vertexData_[idx].boneIdx.x	= t_VertexVec[idx].bone_num[0];
		vertexData_[idx].boneIdx.y	= t_VertexVec[idx].bone_num[1];
		vertexData_[idx].weight.x	= t_VertexVec[idx].bone_weight/100.0f;
		vertexData_[idx].weight.y	= 1.0f - vertexData_[idx].weight.x;
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
	toonIndexVec_.resize(materials_.size());
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
					texPaths_[idx].sphPath = WStringFromString(GetFolderPath(modelPath) + name);
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
			toonIndexVec_[idx] = materials[idx].toon_index;
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