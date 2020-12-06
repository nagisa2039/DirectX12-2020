#include "PMDData.h"
#include "Utility/Tool.h"
#include <sstream>	// 文字列ストリーム用
#include <iomanip>	// 文字列マニピュレータ用(n桁ぞろえやn埋めなど)


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

	// ボーンの読み込み
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

	//表情リスト---------------------------------
	WORD skin_cnt;
	fread_s(&skin_cnt, sizeof(skin_cnt), sizeof(skin_cnt), 1, fp);

#pragma pack(1)
	struct Skin_Data_t
	{
		char skin_name[20]; //　表情名
		DWORD skin_vert_count; // 表情用の頂点数
		BYTE skin_type; // 表情の種類 // 0：base、1：まゆ、2：目、3：リップ、4：その他
	};

	struct Skin_Vert_Data
	{	// base  : 表情用の頂点の番号(頂点リストにある番号)
		// other : 表情用の頂点の番号(baseの番号。skin_vert_index)
		DWORD skin_vert_index; 
		// base  : x, y, z // 表情用の頂点の座標(頂点自体の座標)
		// other : // x, y, z // 表情用の頂点の座標オフセット値(baseに対するオフセット)
		float skin_vert_pos[3]; 
	};
#pragma pack()

	struct SkinData
	{
		BYTE skin_type = 0; // 表情の種類 // 0：base、1：まゆ、2：目、3：リップ、4：その他
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

	//表情枠用表示リスト---------------------------------
	BYTE skin_disp_count;
	fread_s(&skin_disp_count, sizeof(skin_disp_count), sizeof(skin_disp_count), 1, fp);
	vector<WORD> skin_index(skin_disp_count); // 表情番号
	fread_s(skin_index.data(), sizeof(WORD) * skin_disp_count, sizeof(WORD), skin_disp_count, fp);

	//ボーン枠用枠名リスト---------------------------------
	BYTE bone_disp_name_count; // ボーン枠用の枠名数 // センター(1番上に表示される枠)は含まない
	fread_s(&bone_disp_name_count, sizeof(bone_disp_name_count), sizeof(bone_disp_name_count), 1, fp);
	vector<char[50]> dispName(bone_disp_name_count);
	fread_s(dispName.data(), sizeof(char) * 50 * dispName.size(), sizeof(char) * 50, dispName.size(), fp);

	//ボーン枠用表示リスト---------------------------------
	DWORD bone_disp_count; // ボーン枠に表示するボーン数 (枠0(センター)を除く、すべてのボーン枠の合計)
	fread_s(&bone_disp_count, sizeof(bone_disp_count), sizeof(bone_disp_count), 1, fp);

#pragma pack(1)
	struct Bone_Disp_t
	{
		WORD bone_index; // 枠用ボーン番号
		BYTE bone_disp_frame_index;
	};
#pragma pack()
	vector<Bone_Disp_t> bone_disps(bone_disp_count); // 枠用ボーンデータ (3Bytes/bone)
	fread_s(bone_disps.data(), sizeof(Bone_Disp_t) * bone_disps.size(), 
		sizeof(Bone_Disp_t), bone_disps.size(), fp);

	//拡張-----------------------------------------------------------------------------------

	//英語名対応---------------------------------
	BYTE english_name_compatibility;
	fread_s(&english_name_compatibility, sizeof(english_name_compatibility), 
		sizeof(english_name_compatibility), 1, fp);
	if (english_name_compatibility == 1)
	{
		// モデル名
		fseek(fp, 20, SEEK_CUR);
		// コメント
		fseek(fp, 256, SEEK_CUR);

		// ボーン名
		fseek(fp, Cast(long, 20 * bones_.size()), SEEK_CUR);

		// 表情リスト
		fseek(fp, Cast(long, 20 * (skinDatas.size()-1)), SEEK_CUR);

		// ボーン枠用枠名リスト
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
	toonIndexVec_.resize(materials_.size());
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