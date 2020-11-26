#include "ModelActor.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <windows.h>
#include <sstream>

#include "d3dx12.h"
#include "Utility/Tool.h"
#include "Utility/dx12Tool.h"
#include "ModelRenderer.h"
#include "PMXData.h"
#include "PMDData.h"
#include "VMDMotion.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "System/Application.h"
#include "Utility/Input.h"
#include "Material/ModelMaterial.h"
#include "Utility/Cast.h"

using namespace std;
using namespace DirectX; 

namespace
{
	float GetYFramXOnBezier(float x, const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, uint8_t n)
	{
		if (a.x == a.y&&b.x == b.y)
		{
			//計算不要
			return x;
		}

		float t = x;
		const float k0 = 1 + 3 * a.x - 3 * b.x;	//t^3の係数
		const float k1 = 3 * b.x - 6 * a.x;		//t^2の係数
		const float k2 = 3 * a.x;				//tの係数

		//誤差の範囲内かどうかに使用する定数
		constexpr float epsilon = 0.0005f;

		for (int j = 0; j < n; j++)
		{
			auto ft = k0 * t*t*t + k1 * t*t + k2 * t - x;
			if (ft <= epsilon && ft >= -epsilon)
			{
				break;
			}

			//刻む
			t -= ft / 2;
		}

		auto r = 1 - t;
		return t * t*t + 3 * t*t*r*b.y + 3 * t*r*r*a.y;
	}
}

ModelActor::ModelActor(std::string modelPath, Dx12Wrapper& dx12, ModelRenderer& renderer, VMDMotion& vmd)
	:dx12_(dx12), renderer_(renderer), vmdMotion_(vmd)
{
	if (!Init(modelPath))
	{
		assert(false);
	}
	noiseThresholdTL_ = make_unique<TimeLine<float>>(true);
	noiseThresholdTL_->AddKey(0.0f, 1.0f);
	noiseThresholdTL_->AddKey(2.0f, 0.0f);
	noiseThresholdTL_->AddKey(4.0f, 1.0f);
}


ModelActor::~ModelActor()
{
}


bool ModelActor::Init(std::string modelPath)
{
	auto ext = GetExtension(modelPath);
	if (ext == "pmx")
	{
		modelData_ = make_shared<PMXData>(modelPath);
	}
	else if (ext == "pmd")
	{
		modelData_ = make_shared<PMDData>(modelPath);
	}
	else
	{
		assert(false);
	}

	CreateBoneHierarchy();

	// 頂点バッファの作成
	if (!CreateVertexBuffer())
	{
		assert(false);
	}

	// インデックスバッファの作成
	if (!CreateIndexBuffer())
	{
		assert(false);
	}

	// マテリアルの作成
	if (!CreateMaterial())
	{
		assert(false);
	}

	// 座標変換用とボーンのバッファやビューの作成
	if (!CreateConstanteBuffers())
	{
		assert(false);
	}

	// ボーン回転の初期化
	fill(boneMats_.begin(), boneMats_.end(), XMMatrixIdentity());

	// 諸々の初期化
	lastTickTime_ = 0;

	return true;
}


bool ModelActor::CreateBoneHierarchy()
{
	auto bones = modelData_->GetBoneData();
	boneMats_.resize(bones.size());

	for (int idx = 0; idx < static_cast<int>(bones.size()); ++idx)
	{
		auto& b = bones[idx];
		auto& boneNode = boneMap_[b.name];
		boneNode.boneIdx = idx;
		boneNode.startPos = b.startPos;
		boneNode.endPos = b.endPos;
	}

	for (auto& bone : boneMap_)
	{
		auto parentIdx = bones[bone.second.boneIdx].parentIdx;
		if (parentIdx >= bones.size() || parentIdx < 0)
		{
			continue;
		}
		boneMap_[bones[parentIdx].name].children.emplace_back(&bone.second);
	}

	// 全ての親を追加
	if (boneMap_.find(L"全ての親") == boneMap_.end())
	{
		BoneNode node = {};
		boneMats_.emplace_back(XMMatrixIdentity());
		node.boneIdx = static_cast<int>(boneMats_.size() - 1);
		node.children.emplace_back(&boneMap_.find(L"センター")->second);
		boneMap_[L"全ての親"] = node;
	}
	fill(boneMats_.begin(), boneMats_.end(), XMMatrixIdentity());

	return true;
}

void ModelActor::MotionUpdate(const unsigned int motionFrame)
{
	// ボーン回転の初期化
	fill(boneMats_.begin(), boneMats_.end(), XMMatrixIdentity());
	
	auto animation = vmdMotion_.GetAnimation();

	for (auto& anim : animation)
	{
		auto animVec = anim.second;


		// animVecから指定フレームのキーを探す
		auto it = find_if(animVec.rbegin(), animVec.rend(), [motionFrame](const VMDMotion::KeyFrame key)
		{
			return motionFrame >= key.frameNo;
		});

		// 指定フレームのアニメーションが無いのでとばす
		if (it == animVec.rend())
		{
			continue;
		}

		XMFLOAT4 q1 = it->quaternion;
		XMFLOAT4 q2 = it->quaternion;
		// 線形補完用のタイム
		float t = 0.0f;

		// 次のキーフレームのイテレータ
		auto nextIt = it.base();

		auto location = XMLoadFloat3(&it->location);
		if (nextIt != animVec.end())
		{
			q2 = nextIt->quaternion;
			float prevNo = static_cast<float>(it->frameNo);
			float nextNo = static_cast<float>(nextIt->frameNo);
			t = GetYFramXOnBezier((motionFrame - prevNo) / (nextNo - prevNo), nextIt->point1, nextIt->point2, 8);

			location = (1 - t) *  XMLoadFloat3(&it->location) + XMLoadFloat3(&nextIt->location) * t;
		}

		// キーフレームで回転
		RotateBone(WStringFromString(anim.first), location, q1, q2, t);
	}

	//ツリーをトラバース
	XMMATRIX rootmat = XMMatrixIdentity();
	RecursiveMatrixMultiply(boneMap_[L"全ての親"], rootmat);

	// ボーン行列の更新
	// _boneMatsの内容を_mappedBoneにコピーする
	copy(boneMats_.begin(), boneMats_.end(), mappedBones_);

	return;

}

void ModelActor::Update()
{
	// モーションの更新
	auto fps = 30;
	unsigned int motionFrame = static_cast<unsigned int>(static_cast<float>(GetTickCount64() - lastTickTime_) / (1000.0f / fps));
	if (motionFrame > vmdMotion_.GetLastFrame())
	{
		lastTickTime_ = GetTickCount64();
	}
	MotionUpdate(motionFrame);

	auto& input = Application::Instance().GetInput();
	auto Move = [&input = input](const unsigned char keycode, float& target, const float speed)
	{
		if (input.GetButton(keycode))
		{
			target += speed;
		}
	};

	auto trans = GetTransform();
	const float speed = 2.0f;
	Move(DIK_LEFT,	trans.rotate.y, +speed*2.0f);
	Move(DIK_RIGHT, trans.rotate.y, -speed*2.0f);
	Move(DIK_DOWN,	trans.pos.y,	-speed);
	Move(DIK_UP,	trans.pos.y,	+speed);
	Move(DIK_O,		trans.pos.z,	-speed);
	Move(DIK_I,		trans.pos.z,	+speed);

	// 座標更新
	SetTransform(trans);

	noiseThresholdTL_->Update();
	auto value = noiseThresholdTL_->GetValue();
	modelMaterial_->SetConstFloat(
		modelData_->GetMaterialData().size(), value);
}

void ModelActor::Draw()
{
	auto& commandList = dx12_.GetCommand().CommandList();
	auto& dev = dx12_.GetDevice();

	// マテリアルのセット
	modelMaterial_->SetEachDescriptorHeap(commandList);

	// 座標行列用デスクリプタヒープのセット
	SetTransformHeap(7);

	// ボーン用デスクリプタヒープのセット
	commandList.SetDescriptorHeaps(1, boneHeap_.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(8, boneHeap_->GetGPUDescriptorHandleForHeapStart());

	// 設定

	// インデックスバッファのセット
	commandList.IASetIndexBuffer(&ibView_);
	// 頂点バッファビューの設定
	commandList.IASetVertexBuffers(0, 1, &vbView_);

	// 描画コマンドの生成
	commandList.DrawIndexedInstanced(Uint32(modelData_->GetIndexData().size()), 1, 0, 0, 0);
}

void ModelActor::StartAnimation()
{
	lastTickTime_ = GetTickCount64();
}

// 頂点バッファの作成
bool ModelActor::CreateVertexBuffer()
{
	auto vertices = modelData_->GetVertexData();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices[0]) * vertices.size());

	if (FAILED(dx12_.GetDevice().CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexBuffer_.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	// データ転送
	// vertexBufferにverMapの内容を書き込む
	// 頂点書き込み
	ModelData::Vertex* verMap = nullptr;
	if (FAILED(vertexBuffer_->Map(0, nullptr, (void**)&verMap)))
	{
		return false;
	}
	copy(vertices.begin(), vertices.end(), verMap);
	// 終了したのでアンマップ
	vertexBuffer_->Unmap(0, nullptr);


	// 頂点バッファビューの設定
	// 頂点バッファのGPUにおけるアドレスを記録
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();

	// データ全体のサイズ指定
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	// 1頂点当たりのバイト数指定	(全体のバイト数 / 頂点数)
	vbView_.StrideInBytes = static_cast<UINT>(vbView_.SizeInBytes / vertices.size());

	return true;
}

bool ModelActor::CreateIndexBuffer()
{
	auto indices = modelData_->GetIndexData();

	auto bufferSize = sizeof(indices[0]) * indices.size();
	if (FAILED(dx12_.GetDevice().CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexBuffer_.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	// データ転送
	// indexBufferにverMapの内容を書き込む
	// 頂点書き込み
	uint16_t* indexMap = nullptr;
	if (FAILED(indexBuffer_->Map(0, nullptr, (void**)&indexMap)))
	{
		return false;
	}
	copy(indices.begin(), indices.end(), indexMap);
	// 終了したのでアンマップ
	indexBuffer_->Unmap(0, nullptr);


	// インデックスバッファビューの設定
	//バッファのGPUアドレス
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();

	// フォーマット
	ibView_.Format = DXGI_FORMAT_R16_UINT;

	// 総サイズ
	ibView_.SizeInBytes = static_cast<UINT>(bufferSize);

	return true;
}

bool ModelActor::CreateMaterial()
{
	auto& dev = dx12_.GetDevice();
	auto materials = modelData_->GetMaterialData();

	auto texPaths = modelData_->GetTexturePaths();

	std::vector<MaterialBase> meterialBaseVec(materials.size());

	auto FLOAT3 = [](const XMFLOAT4& float4)
	{
		return XMFLOAT3(float4.x, float4.y, float4.z);
	};

	std::vector<float> constFloatVec(materials.size()+1);
	for (int i = 0; auto & materialBase : meterialBaseVec)
	{
		const auto& mat = materials[i];
		materialBase = MaterialBase{ mat.diffuse,
			mat.specular, mat.ambient, mat.power, -1 };
		constFloatVec[i] = Float(mat.indeicesNum);
		i++;
	}
	constFloatVec[materials.size()] = 0.0f;


	auto& texLoader = dx12_.GetTexLoader();
	auto GetTexture = [&texLoader = texLoader](const std::wstring& path, const int failedIdx)
	{
		if (path != L"")
		{
			int handle = texLoader.LoadGraph(path);
			if (handle == FAILED)
			{
				return failedIdx;
			}
			return handle;
		}
		return failedIdx;
	};

	const int stride = 4;
	std::vector<int> addTexVec(stride * materials.size()+1);
	auto dummyTexHandles = texLoader.GetDummyTextureHandles();
	for (int j = 0; j < addTexVec.size()-1; j+= stride)
	{
		auto matIdx = j / stride;
		meterialBaseVec[matIdx].textureIndex = GetTexture(texPaths[matIdx].texPath,  dummyTexHandles.whiteTexH);
		addTexVec[Uint64(j) + 0]			 = GetTexture(texPaths[matIdx].sphPath,  dummyTexHandles.whiteTexH);
		addTexVec[Uint64(j) + 1]			 = GetTexture(texPaths[matIdx].spaPath,  dummyTexHandles.blackTexH);
		addTexVec[Uint64(j) + 2]			 = GetTexture(texPaths[matIdx].subPath,  dummyTexHandles.whiteTexH);
		addTexVec[Uint64(j) + 3]			 = GetTexture(texPaths[matIdx].toonPath, dummyTexHandles.whiteTexH);
	}
	addTexVec[Uint64(stride) * materials.size()] = texLoader.LoadGraph(L"Resource/Image/noise.png");

	modelMaterial_ = make_unique<ModelMaterial>(meterialBaseVec, addTexVec, constFloatVec);

	return true;
}

void ModelActor::RecursiveMatrixMultiply(BoneNode & node, DirectX::XMMATRIX& inMat)
{
	boneMats_[node.boneIdx] *= inMat;
	for (auto& cnode : node.children)
	{
		RecursiveMatrixMultiply(*cnode, boneMats_[node.boneIdx]);
	}
}

void ModelActor::RotateBone(std::wstring boneName, DirectX::XMVECTOR location, DirectX::XMFLOAT4& q1, DirectX::XMFLOAT4& q2, float t)
{
	auto bone = boneMap_[boneName];
	auto vec = XMLoadFloat3(&bone.startPos);
	auto quaternion1 = XMLoadFloat4(&q1);
	auto quaternion2 = XMLoadFloat4(&q2);

	vec = XMVector3Transform(vec, boneMats_[bone.boneIdx]);

	XMMATRIX mat =
		XMMatrixTranslationFromVector(XMVectorScale(vec, -1))
		*XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion1, quaternion2, t))
		*XMMatrixTranslationFromVector(vec);

	mat *= XMMatrixTranslationFromVector(location);

	boneMats_[bone.boneIdx] *= mat;
}

bool ModelActor::CreateConstanteBuffers()
{
	auto& dev = dx12_.GetDevice();
	// ボーンヒープ作成
	CreateDescriptorHeap(&dev, boneHeap_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);

	// ボーンの定数バッファの作成
	CreateUploadBuffer(&dev, boneCB_, static_cast<UINT>(sizeof(boneMats_[0]) * boneMats_.size()));
	boneCB_->Map(0, nullptr, (void**)&mappedBones_);

	// 定数バッファビューの作成
	CreateConstantBufferView(&dev, boneCB_, boneHeap_->GetCPUDescriptorHandleForHeapStart());

	return true;
}