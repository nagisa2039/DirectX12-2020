#include "PMDData.h"
#include "Utility/Tool.h"
#include <sstream>	// 文字列ストリーム用
#include <iomanip>	// 文字列マニピュレータ用(n桁ぞろえやn埋めなど)


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


	// ヘッダの読み込み
	// #pragma pack(1) アライメントを１バイトにする
#pragma pack(1)
	struct PMDHeader
	{
		char signature[3];	// 3
		// パディング1が入る
		float version;		// 4
		char model_name[20];//	20
		char comment[256];//256
	};//283バイト
#pragma pack()

	PMDHeader pmdheader;
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	// 頂点
	LoadVertex(fp);

	// 頂点インデックス
	LoadVertexIndex(fp);

	// マテリアルの読み込み
	LoadMaterial(fp, modelPath);

	SetVertexMaterialIndex();

	// ボーンの読み込み
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
	// 頂点
#pragma pack(1)
	struct t_Vertex
	{
		XMFLOAT3 pos;//12
		XMFLOAT3 normal_vec;//12
		XMFLOAT2 uv;//8
		uint16_t bone_num[2];//4
		uint8_t bone_weight;//1
		uint8_t edge_flag;	//1
	};//38バイト
#pragma pack()

	// 頂点
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
	// マテリアル
#pragma pack(1)
	struct t_Material
	{
		XMFLOAT4 diffuse_color; // dr, dg, db, da
		float specularity;//スペキュラ乗数
		XMFLOAT3 specular_color; // sr, sg, sb // 光沢色
		XMFLOAT3 mirror_color; // mr, mg, mb // 環境色(ambient)
		uint8_t toon_index; // toon??.bmp //
		uint8_t edge_flag; // 輪郭、影
						   //パディング2個が予想される…ここまでで46バイト
		uint32_t face_vert_count; // 面頂点数
		char texture_file_name[20]; // テクスチャファイル名
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
		// モデルからの相対パスをprojからの相対パスに変換する
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
	struct t_bone	// 39バイト
	{
		char bone_name[20]; // ボーン名
		uint16_t parent_bone_index; // 親ボーン番号(ない場合は0xFFFF)
		uint16_t tail_pos_bone_index; // tail位置のボーン番号(チェーン末端の場合は0xFFFF 0 →補足2) // 親：子は1：多なので、主に位置決め用
		uint8_t bone_type; // ボーンの種類
		uint16_t ik_parent_bone_index; // IKボーン番号(影響IKボーン。ない場合は0)
		XMFLOAT3 bone_head_pos; // x, y, z // ボーンのヘッド
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