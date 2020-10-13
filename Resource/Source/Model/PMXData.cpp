#include "PMXData.h"

#include "Utility/Tool.h"
#include <functional>
#include <sstream>	// 文字列ストリーム用
#include <iomanip>	// 文字列マニピュレータ用(n桁ぞろえやn埋めなど)

using namespace std;
using namespace DirectX;

PMXData::PMXData(std::string modelPath)
{
	LoadFromPMX(modelPath);
}


PMXData::~PMXData()
{
}

bool PMXData::LoadFromPMX(std::string modelPath)
{
	FILE* fp = nullptr;
	fopen_s(&fp, modelPath.c_str(), "rb");

	assert(fp != nullptr);
	if (fp == nullptr)return false;

	fseek(fp, 8, SEEK_CUR);

	uint8_t length = 0;
	fread(&length, sizeof(length), 1, fp);

	_info.resize(length);
	fread(_info.data(), length, 1, fp);

	wstring modelName;
	ReadTextBuf(modelName, fp);

	wstring modelNameEng;
	ReadTextBuf(modelNameEng, fp);

	wstring com;
	ReadTextBuf(com, fp);

	wstring comEng;
	ReadTextBuf(comEng, fp);

	// 頂点の読み込み
	LoadVertex(fp);

	// 頂点インデックスの読み込み
	LoadVertexIndex(fp);

	// マテリアルの読み込み
	LoadMaterial(fp, modelPath);

	// ボーンの読み込み
	LoadBone(fp);


	fclose(fp);

	return true;
}

void PMXData::LoadVertexIndex(FILE * fp)
{
	int indexNum = 0;
	fread(&indexNum, sizeof(int), 1, fp);

	_indexData.resize(indexNum);

	for (int idx = 0; idx < indexNum; idx++)
	{
		fread(&_indexData[idx], _info[2], 1, fp);
	}
}

void PMXData::LoadVertex(FILE * fp)
{
	// 頂点
	int vertNum = 0;
	fread(&vertNum, sizeof(int), 1, fp);

	_vertexData.resize(vertNum);

	enum WeightType
	{
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
	};

	function<void(FILE*, int, ModelData::Vertex&)> weightFun[4];
	weightFun[BDEF1] = [](FILE * fp, int boneIdexSize, ModelData::Vertex& vert)
	{
		int boneNum = 0;
		fread(&boneNum, boneIdexSize, 1, fp);
		vert.boneIdx.x = boneNum;
		vert.weight.x = 1.0f;
	};


	weightFun[BDEF2] = [](FILE * fp, int boneIdexSize, ModelData::Vertex& vert)
	{
		int boneNum1 = 0;
		fread(&boneNum1, boneIdexSize, 1, fp);
		int boneNum2 = 0;
		fread(&boneNum2, boneIdexSize, 1, fp);

		float weight = 0.0f;
		fread(&weight, sizeof(weight), 1, fp);

		vert.boneIdx.x = boneNum1;
		vert.weight.x = weight;
		vert.boneIdx.y = boneNum2;
		vert.weight.y = 1.0f - weight;
	};

	weightFun[BDEF4] = [](FILE * fp, int boneIdexSize, ModelData::Vertex& vert)
	{
		float total = 0.0f;
		pair<int, float> bone[4];
		for (int idx = 0; idx < 4; idx++)
		{
			fread(&bone[idx].first, boneIdexSize, 1, fp);
			fread(&bone[idx].second, sizeof(bone[idx].second), 1, fp);
			total += bone[idx].second;
		}

		vert.boneIdx.x = bone[0].first;
		vert.weight.x = bone[0].second / total;
		vert.boneIdx.y = bone[1].first;
		vert.weight.y = bone[1].second / total;
		vert.boneIdx.z = bone[2].first;
		vert.weight.z = bone[2].second / total;
		vert.boneIdx.w = bone[3].first;
		vert.weight.w = bone[3].second / total;
	};

	weightFun[SDEF] = [](FILE * fp, int boneIdexSize, ModelData::Vertex& vert)
	{
		int boneNum1 = 0;
		fread(&boneNum1, boneIdexSize, 1, fp);
		int boneNum2 = 0;
		fread(&boneNum2, boneIdexSize, 1, fp);

		float weight = 0.0f;
		fread(&weight, sizeof(weight), 1, fp);

		vert.boneIdx.x = boneNum1;
		vert.weight.x = weight;
		vert.boneIdx.y = boneNum2;
		vert.weight.y = 1.0f - weight;

		// 今度やる
		XMFLOAT3 c;
		fread(&c, sizeof(c), 1, fp);
		XMFLOAT3 r0;
		fread(&r0, sizeof(r0), 1, fp);
		XMFLOAT3 r1;
		fread(&r1, sizeof(r1), 1, fp);
	};

	for (int j = 0; j < vertNum; j++)
	{
		fread(&_vertexData[j].pos, sizeof(_vertexData[j].pos), 1, fp);
		fread(&_vertexData[j].normal, sizeof(_vertexData[j].normal), 1, fp);
		fread(&_vertexData[j].uv, sizeof(_vertexData[j].uv), 1, fp);

		std::vector<DirectX::XMFLOAT3> adduv;
		adduv.resize(_info[1]);
		for (int k = 0; k < _info[1]; k++)
		{
			fread(&adduv[k], sizeof(adduv[k]), 1, fp);
		}

		uint8_t boneformat = 0;
		fread(&boneformat, sizeof(boneformat), 1, fp);
		weightFun[boneformat](fp, _info[5], _vertexData[j]);

		float edge = 0;
		fread(&edge, sizeof(edge), 1, fp);
	}
}

void PMXData::ReadTextBuf(std::wstring& wstrBuf, FILE * fp)
{
	int buffLen = 0;
	fread(&buffLen, sizeof(buffLen), 1, fp);

	if (_info[0] == 0)
	{
		wstrBuf.resize(buffLen / 2);
		fread(&wstrBuf[0], buffLen, 1, fp);
	}
	else
	{
		string str;
		str.resize(buffLen);
		fread(&str[0], buffLen, 1, fp);
		wstrBuf = WStringFromString(str);
	}
}

void PMXData::LoadMaterial(FILE * fp, std::string &modelPath)
{
	int texNum = 0;
	fread(&texNum, sizeof(texNum), 1, fp);

	// テクスチャパスの読み込み
	vector<wstring> texPathTable(texNum);

	for (auto& texPath : texPathTable)
	{
		ReadTextBuf(texPath, fp);
		auto idx = texPath.find(L"\\");
		if (idx < texPath.size())
		{
			auto path1 = texPath.substr(0,idx);
			auto path2 = texPath.substr(idx+1);
			texPath = path1 + L"/" + path2;
		}
	}

	// マテリアル
#pragma pack(1)
	struct t_Material
	{
		XMFLOAT4 diffuse_color; // dr, dg, db, da
		XMFLOAT3 specular_color; // sr, sg, sb // 光沢色
		float specularity;//スペキュラ乗数
		XMFLOAT3 ambient; // mr, mg, mb // 環境色(ambient)
		uint8_t drawFlag; // toon??.bmp //
		XMFLOAT4 edge_color; // エッジ色
		float edgeSize;	// エッジサイズ
	};
#pragma pack()

	int materialNum = 0;
	fread(&materialNum, sizeof(int), 1, fp);

	_materials.resize(materialNum);
	_texPaths.resize(materialNum);

	for (int idx = 0; idx < materialNum; idx++)
	{
		wstring matName;
		ReadTextBuf(matName, fp);

		wstring matNameEng;
		ReadTextBuf(matNameEng, fp);

		t_Material tMat;
		fread(&tMat, sizeof(t_Material), 1, fp);

		uint32_t texIdx = 0;
		fread(&texIdx, _info[3], 1, fp);

		uint32_t spIdx = 0;
		fread(&spIdx, _info[3], 1, fp);

		uint8_t spMode = 0;
		fread(&spMode, sizeof(spMode), 1, fp);

		uint8_t shareToomFlag = 0;
		fread(&shareToomFlag, sizeof(shareToomFlag), 1, fp);

		uint32_t toonIdx = 0;
		if (shareToomFlag == 0)
		{
			fread(&toonIdx, _info[3], 1, fp);
		}
		else
		{
			fread(&toonIdx, sizeof(uint8_t), 1, fp);
		}

		wstring memo;
		ReadTextBuf(memo, fp);

		uint32_t indexNum = 0;
		fread(&indexNum, sizeof(indexNum), 1, fp);

		auto& mat = _materials[idx];
		mat.diffuse = tMat.diffuse_color;
		mat.specular = tMat.specular_color;
		mat.ambient = tMat.ambient;
		mat.power = tMat.specularity;
		mat.indeicesNum = indexNum;

		if (texPathTable.size() > texIdx)
		{
			_texPaths[idx].texPath = WStringFromString(GetFolderPath(modelPath)) + texPathTable[texIdx];
		}

		if (texPathTable.size() > spIdx)
		{
			switch (spMode)
			{
			case 1:
				_texPaths[idx].sphPath = WStringFromString(GetFolderPath(modelPath)) + texPathTable[spIdx];
				break;

			case 2:
				_texPaths[idx].spaPath = WStringFromString(GetFolderPath(modelPath)) + texPathTable[spIdx];
				break;

			case 3:
				_texPaths[idx].subPath = WStringFromString(GetFolderPath(modelPath)) + texPathTable[spIdx];
				break;

			case 0:
			default:
				break;
			}
		}

		if (shareToomFlag)
		{
			if (toonIdx < 10)
			{
				ostringstream oss;
				oss << "toon/toon" << setw(2) << setfill('0') << static_cast<int>(toonIdx + 1) << ".bmp";
				_texPaths[idx].toonPath = WStringFromString(oss.str());
			}
		}
		else
		{
			if (texPathTable.size() > toonIdx)
			{
				_texPaths[idx].toonPath = WStringFromString(GetFolderPath(modelPath)) + texPathTable[toonIdx];
			}
		}
	}
}

void PMXData::LoadBone(FILE * fp)
{
	int boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	struct IKLink
	{
		uint32_t boneIdx;
		unsigned char radLimit;
		XMFLOAT3 downLimit;
		XMFLOAT3 upLimit;
	};

	struct Bone
	{
		wstring boneName;
		XMFLOAT3 startpos;
		XMFLOAT3 endPos;
		uint32_t endBoneIdx;
		uint32_t parentBoneIdx;
		int transformHierarchy;
		uint16_t bitFlag;
		uint32_t rotateGrant;	//	回転付与
		uint32_t moveGrant;	//	移動付与
		float grandParsent;		// 付与率
		XMFLOAT3 axisVec;	// 固定軸
		XMFLOAT3 localAxisX;
		XMFLOAT3 localAxisY;
		int key;	// 外部親変形
		uint32_t ikBoneIdx;
		int roopCnt;
		float radLimit;
		vector<IKLink> ikLinkVec;
	};

	vector<Bone> bones(boneNum);
	_bones.resize(boneNum);

	for (int idx = 0; idx < boneNum; idx++)
	{
		auto bone = bones[idx];
		bone = {};
		ReadTextBuf(bone.boneName, fp);
		wstring boneNameEng;
		ReadTextBuf(boneNameEng, fp);

		// ボーンの開始座標
		fread(&bone.startpos, sizeof(bone.startpos), 1, fp);
		// 親ボーンのインデックス
		fread(&bone.parentBoneIdx, _info[5], 1, fp);
		// 変形階層
		fread(&bone.transformHierarchy, sizeof(bone.transformHierarchy), 1, fp);
		// ビットフラグ
		fread(&bone.bitFlag, sizeof(bone.bitFlag), 1, fp);

		// 接続先
		if (bone.bitFlag & 0x0001)
		{
			fread(&bone.endBoneIdx, _info[5], 1, fp);
		}
		else
		{
			fread(&bone.endPos, sizeof(bone.endPos), 1, fp);
		}
		// 回転付与 または 移動付与
		if ((bone.bitFlag & 0x0100) | (bone.bitFlag & 0x0200))
		{
			fread(bone.bitFlag & 0x0100 ? &bone.rotateGrant : &bone.moveGrant, _info[5], 1, fp);
			fread(&bone.grandParsent, sizeof(bone.grandParsent), 1, fp);
		}
		// 軸固定
		if (bone.bitFlag & 0x0400)
		{
			fread(&bone.axisVec, sizeof(bone.axisVec), 1, fp);
		}
		// ローカル軸
		if (bone.bitFlag & 0x0800)
		{
			fread(&bone.localAxisX, sizeof(bone.localAxisX), 1, fp);
			fread(&bone.localAxisY, sizeof(bone.localAxisY), 1, fp);
		}
		// 外部親変形
		if (bone.bitFlag & 0x2000)
		{
			fread(&bone.key, sizeof(bone.key), 1, fp);
		}
		// IK
		if (bone.bitFlag & 0x0020)
		{
			fread(&bone.ikBoneIdx, _info[5], 1, fp);
			fread(&bone.roopCnt, sizeof(bone.roopCnt), 1, fp);
			fread(&bone.radLimit, sizeof(bone.radLimit), 1, fp);

			int linkCnt = 0;
			fread(&linkCnt, sizeof(linkCnt), 1, fp);
			bone.ikLinkVec.resize(linkCnt);

			for (auto& ikLink : bone.ikLinkVec)
			{
				fread(&ikLink.boneIdx, _info[5], 1, fp);
				fread(&ikLink.radLimit, sizeof(ikLink.radLimit), 1, fp);
				if (ikLink.radLimit)
				{
					fread(&ikLink.downLimit, sizeof(ikLink.downLimit), 1, fp);
					fread(&ikLink.upLimit, sizeof(ikLink.upLimit), 1, fp);
				}
			}
		}

		_bones[idx].name = bone.boneName;
		_bones[idx].parentIdx = bone.parentBoneIdx;
		_bones[idx].startPos = bone.startpos;
		_bones[idx].endPos = bone.endPos;
	}
}