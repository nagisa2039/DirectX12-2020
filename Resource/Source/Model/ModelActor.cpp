#include "ModelActor.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <windows.h>

#include "d3dx12.h"
#include "Utility/Tool.h"
#include "Utility/dx12Tool.h"
#include "ModelRenderer.h"
#include "PMXData.h"
#include "PMDData.h"
#include "VMDMotion.h"
#include "System/Dx12Wrapper.h"

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
	:_dx12(dx12), _renderer(renderer), _vmdMotion(vmd)
{
	if (!Init(modelPath))
	{
		assert(false);
	}
}


ModelActor::~ModelActor()
{
}


bool ModelActor::Init(std::string modelPath)
{
	auto ext = GetExtension(modelPath);
	if (ext == "pmx")
	{
		_modelData = make_shared<PMXData>(modelPath);
	}
	else if (ext == "pmd")
	{
		_modelData = make_shared<PMDData>(modelPath);
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

	// マテリアルバッファの作成
	if (!CreateMaterialBuffer())
	{
		assert(false);
	}

	// マテリアルのテクスチャバッファの作成
	if (!CreateMaterialTextureBuffer())
	{
		assert(false);
	}

	// マテリアルのビュー作成
	if (!CreateMaterialCBV())
	{
		assert(false);
	}

	// 座標変換用とボーンのバッファやビューの作成
	if (!CreateConstanteBuffers())
	{
		assert(false);
	}

	

	// ボーン回転の初期化
	fill(_boneMats.begin(), _boneMats.end(), XMMatrixIdentity());

	// 諸々の初期化
	_trans.pos = { 0,0,0 };
	_trans.rotate = { 0,0,0 };
	_lastTickTime = 0;

	return true;
}


bool ModelActor::CreateBoneHierarchy()
{
	auto bones = _modelData->GetBoneData();
	_boneMats.resize(bones.size());

	for (int idx = 0; idx < static_cast<int>(bones.size()); ++idx)
	{
		auto& b = bones[idx];
		auto& boneNode = _boneMap[b.name];
		boneNode.boneIdx = idx;
		boneNode.startPos = b.startPos;
		boneNode.endPos = b.endPos;
	}

	for (auto& bone : _boneMap)
	{
		auto parentIdx = bones[bone.second.boneIdx].parentIdx;
		if (parentIdx >= bones.size() || parentIdx < 0)
		{
			continue;
		}
		_boneMap[bones[parentIdx].name].children.emplace_back(&bone.second);
	}

	// 全ての親を追加
	if (_boneMap.find(L"全ての親") == _boneMap.end())
	{
		BoneNode node = {};
		_boneMats.emplace_back(XMMatrixIdentity());
		node.boneIdx = static_cast<int>(_boneMats.size() - 1);
		node.children.emplace_back(&_boneMap.find(L"センター")->second);
		_boneMap[L"全ての親"] = node;
	}
	fill(_boneMats.begin(), _boneMats.end(), XMMatrixIdentity());

	return true;
}

void ModelActor::MotionUpdate(const unsigned int motionFrame)
{
	// ボーン回転の初期化
	fill(_boneMats.begin(), _boneMats.end(), XMMatrixIdentity());
	
	auto animation = _vmdMotion.GetAnimation();

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
	RecursiveMatrixMultiply(_boneMap[L"全ての親"], rootmat);

	// ボーン行列の更新
	// _boneMatsの内容を_mappedBoneにコピーする
	copy(_boneMats.begin(), _boneMats.end(), _mappedBones);

	return;

}

void ModelActor::Update()
{
	// モーションの更新
	auto fps = 30;
	unsigned int motionFrame = static_cast<unsigned int>(static_cast<float>(GetTickCount() - _lastTickTime) / (1000.0f / fps));
	if (motionFrame > _vmdMotion.GetLastFrame())
	{
		_lastTickTime = GetTickCount64();
	}
	MotionUpdate(motionFrame);

	// 座標更新
	const float deg2rad = (XM_PI / 180.0f);
	*_mappedTrans =
		XMMatrixRotationRollPitchYaw(_trans.rotate.x * deg2rad, _trans.rotate.y * deg2rad, _trans.rotate.z * deg2rad)
		*XMMatrixTranslation(_trans.pos.x, _trans.pos.y, _trans.pos.z);
}

void ModelActor::Draw(bool isShadow)
{
	auto handleW = _worldHeap->GetGPUDescriptorHandleForHeapStart();

	// 座標行列用デスクリプタヒープのセット
	auto& commandList = _dx12.GetCommand().CommandList();
	auto& dev = _dx12.GetDevice();

	commandList.SetDescriptorHeaps(1, _worldHeap.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(2, handleW);
	handleW.ptr += dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// マテリアル用デスクリプタヒープの設定
	commandList.SetDescriptorHeaps(1, _materialHeap.GetAddressOf());

	// インデックスバッファのセット
	commandList.IASetIndexBuffer(&_ibView);
	// 頂点バッファビューの設定
	commandList.IASetVertexBuffers(0, 1, &_vbView);

	// 描画コマンドの生成
	// 第一引数 インデックス数
	int idxOffset = 0;
	auto handle = _materialHeap->GetGPUDescriptorHandleForHeapStart();

	if (isShadow)
	{
		commandList.DrawIndexedInstanced(static_cast<UINT>(_modelData->GetIndexData().size()), 1, 0, 0, 0);
	}
	else
	{
		auto materials = _modelData->GetMaterialData();
		for (const auto& material : materials)
		{
			commandList.SetGraphicsRootDescriptorTable(0, handle);
			handle.ptr += static_cast<uint64_t>(dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)) * 6;
			int idxNum = material.indeicesNum;
			commandList.DrawIndexedInstanced(idxNum, 1, idxOffset, 0, 0);
			idxOffset += idxNum;
		}
	}
}

void ModelActor::StartAnimation()
{
	_lastTickTime = GetTickCount64();
}

ModelActor::Transform & ModelActor::GetTransform()
{
	return _trans;
}

void ModelActor::SetTransform(const Transform & trans)
{
	_trans = trans;
}

// 頂点バッファの作成
bool ModelActor::CreateVertexBuffer()
{
	auto vertices = _modelData->GetVertexData();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices[0]) * vertices.size());

	if (FAILED(_dx12.GetDevice().CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_vertexBuffer.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	// データ転送
	// vertexBufferにverMapの内容を書き込む
	// 頂点書き込み
	ModelData::Vertex* verMap = nullptr;
	if (FAILED(_vertexBuffer->Map(0, nullptr, (void**)&verMap)))
	{
		return false;
	}
	copy(vertices.begin(), vertices.end(), verMap);
	// 終了したのでアンマップ
	_vertexBuffer->Unmap(0, nullptr);


	// 頂点バッファビューの設定
	// 頂点バッファのGPUにおけるアドレスを記録
	_vbView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();

	// データ全体のサイズ指定
	_vbView.SizeInBytes = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	// 1頂点当たりのバイト数指定	(全体のバイト数 / 頂点数)
	_vbView.StrideInBytes = static_cast<UINT>(_vbView.SizeInBytes / vertices.size());

	return true;
}

bool ModelActor::CreateIndexBuffer()
{
	auto indices = _modelData->GetIndexData();

	auto bufferSize = sizeof(indices[0]) * indices.size();
	if (FAILED(_dx12.GetDevice().CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_indexBuffer.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	// データ転送
	// indexBufferにverMapの内容を書き込む
	// 頂点書き込み
	uint16_t* indexMap = nullptr;
	if (FAILED(_indexBuffer->Map(0, nullptr, (void**)&indexMap)))
	{
		return false;
	}
	copy(indices.begin(), indices.end(), indexMap);
	// 終了したのでアンマップ
	_indexBuffer->Unmap(0, nullptr);


	// インデックスバッファビューの設定
	//バッファのGPUアドレス
	_ibView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();

	// フォーマット
	_ibView.Format = DXGI_FORMAT_R16_UINT;

	// 総サイズ
	_ibView.SizeInBytes = static_cast<UINT>(bufferSize);

	return true;
}

bool ModelActor::CreateMaterialTextureBuffer()
{
	auto texPaths = _modelData->GetTexturePaths();
	_textures.resize(texPaths.size());

	bool debug = false;

	auto GetTexture = [&dx12 = _dx12](const std::wstring & path, TextureResorce & texRes)
	{
		if (path != L"")
		{
			dx12.GetTextureResouse(path, texRes);
		}
	};

	for (unsigned int j = 0; j < texPaths.size(); j++)
	{
		GetTexture(texPaths[j].texPath,  _textures[j].texResource);
		GetTexture(texPaths[j].sphPath,  _textures[j].sphResource);
		GetTexture(texPaths[j].spaPath,  _textures[j].spaResource);
		GetTexture(texPaths[j].subPath,	 _textures[j].subResource);
		GetTexture(texPaths[j].toonPath, _textures[j].toonResource);
	}
	return true;
}

bool ModelActor::CreateMaterialBuffer()
{
	auto materials = _modelData->GetMaterialData();
	// バッファの作成
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	auto aliSize = AlignmentValue(sizeof(MaterialForBuffer),
		D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(aliSize * materials.size());
	if (FAILED(_dx12.GetDevice().CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_materialBuffer.ReleaseAndGetAddressOf()))))
	{
		return false;
	}

	// マップでコピー
	uint8_t* mappedMaterial = nullptr;
	_materialBuffer->Map(0, nullptr, (void**)&mappedMaterial);
	for (auto& material : materials)
	{
		((MaterialForBuffer*)mappedMaterial)->diffuse = material.diffuse;
		((MaterialForBuffer*)mappedMaterial)->specular =
			XMFLOAT4(material.specular.x, material.specular.y, material.specular.z, 1);
		((MaterialForBuffer*)mappedMaterial)->ambient =
			XMFLOAT4(material.ambient.x, material.ambient.y, material.ambient.z, 1);
		((MaterialForBuffer*)mappedMaterial)->power = material.power;
		mappedMaterial += aliSize;
	}
	_materialBuffer->Unmap(0, nullptr);

	return true;
}

bool ModelActor::CreateMaterialCBV()
{
	auto materials = _modelData->GetMaterialData();

	// デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = static_cast<UINT>(materials.size() * 6);	// マテリアルとテクスチャとsphとspaとtoon
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	auto& dev = _dx12.GetDevice();
	if (FAILED(dev.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_materialHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// 定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
	viewDesc.BufferLocation = _materialBuffer->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(_materialBuffer->GetDesc().Width / materials.size());
	auto handle = _materialHeap->GetCPUDescriptorHandleForHeapStart();
	auto heapStride = dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto bufferStride
		= AlignmentValue(sizeof(MaterialForBuffer), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//後述
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CreateMaterialTextureView(viewDesc, handle, heapStride, bufferStride, srvDesc);

	return true;
}

void ModelActor::CreateMaterialTextureView(D3D12_CONSTANT_BUFFER_VIEW_DESC& viewDesc, D3D12_CPU_DESCRIPTOR_HANDLE& handle, const UINT& heapStride, unsigned int bufferStride, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
	auto& dummyTextures = _dx12.GetDummyTextures();
	auto& dev = _dx12.GetDevice();

	auto CreateShaderResourceView = [&dev, &srvDesc, &handle, &dummyTextures, &heapStride](TextureResorce& texRes)
	{
		// テクスチャ
		if (texRes.resource.buffer.Get() != nullptr)
		{
			srvDesc.Format = texRes.resource.buffer->GetDesc().Format;
			dev.CreateShaderResourceView(texRes.resource.buffer.Get(), &srvDesc, handle);
		}
		else
		{
			srvDesc.Format = dummyTextures.whiteTex.buffer->GetDesc().Format;
			dev.CreateShaderResourceView(dummyTextures.whiteTex.buffer.Get(), &srvDesc, handle);
		}
		handle.ptr += heapStride;
	};

	for (unsigned int j = 0; j < _textures.size(); j++)
	{
		dev.CreateConstantBufferView(&viewDesc, handle);
		handle.ptr += heapStride;
		viewDesc.BufferLocation += bufferStride;

		// テクスチャ
		CreateShaderResourceView(_textures[j].texResource);
		// スフィアマップ(乗算)
		CreateShaderResourceView(_textures[j].sphResource);
		// スフィアマップ(加算)
		CreateShaderResourceView(_textures[j].spaResource);
		// 追加テクスチャ
		CreateShaderResourceView(_textures[j].subResource);
		// toon
		CreateShaderResourceView(_textures[j].toonResource);
	}
}

void ModelActor::RecursiveMatrixMultiply(BoneNode & node, DirectX::XMMATRIX& inMat)
{
	_boneMats[node.boneIdx] *= inMat;
	for (auto& cnode : node.children)
	{
		RecursiveMatrixMultiply(*cnode, _boneMats[node.boneIdx]);
	}
}

void ModelActor::RotateBone(std::wstring boneName, DirectX::XMVECTOR location, DirectX::XMFLOAT4& q1, DirectX::XMFLOAT4& q2, float t)
{
	auto bone = _boneMap[boneName];
	auto vec = XMLoadFloat3(&bone.startPos);
	auto quaternion1 = XMLoadFloat4(&q1);
	auto quaternion2 = XMLoadFloat4(&q2);

	vec = XMVector3Transform(vec, _boneMats[bone.boneIdx]);

	XMMATRIX mat =
		XMMatrixTranslationFromVector(XMVectorScale(vec, -1))
		*XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion1, quaternion2, t))
		*XMMatrixTranslationFromVector(vec);

	mat *= XMMatrixTranslationFromVector(location);

	_boneMats[bone.boneIdx] *= mat;
}

bool ModelActor::CreateConstanteBuffers()
{
	// 座標の定数バッファの作成
	auto& dev = _dx12.GetDevice();
	CreateConstantBuffer(&dev, _transCB, sizeof(*_mappedTrans));
	_transCB->Map(0, nullptr, (void**)&_mappedTrans);

	// 座標のヒープ作成
	CreateDescriptorHeap(&dev, _worldHeap, 2);

	// 定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
	viewDesc.BufferLocation = _transCB->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(_transCB->GetDesc().Width);
	auto handle = _worldHeap->GetCPUDescriptorHandleForHeapStart();
	dev.CreateConstantBufferView(&viewDesc, handle);
	handle.ptr += dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// ボーンの定数バッファの作成
	CreateConstantBuffer(&dev, _boneCB, static_cast<UINT>(sizeof(_boneMats[0]) * _boneMats.size()));
	_boneCB->Map(0, nullptr, (void**)&_mappedBones);

	// 定数バッファビューの作成
	viewDesc = {};
	viewDesc.BufferLocation = _boneCB->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = static_cast<UINT>(_boneCB->GetDesc().Width);
	dev.CreateConstantBufferView(&viewDesc, handle);

	return true;
}