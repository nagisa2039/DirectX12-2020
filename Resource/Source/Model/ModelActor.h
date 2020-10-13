#pragma once
#include <vector>
#include <DirectXMath.h>
#include <string>
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include "ModelData.h"
#include <memory>
#include "System/TextureStruct.h"

class VMDMotion;
class Dx12Wrapper;
class ModelRenderer;
using Microsoft::WRL::ComPtr;

class ModelActor
{
public:
	struct Transform
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 rotate;
	};

	ModelActor(std::string modelPath, Dx12Wrapper& dx12, ModelRenderer& rnderer, VMDMotion& vmd);
	~ModelActor();

	// 初期化(モデルのファイルパス)
	bool Init(std::string modelPath);
	// 更新
	void Update();
	// 描画(影として描画するか)
	void Draw(bool isShadow = false);
	// アニメーションを開始する
	void StartAnimation();
	// transform取得
	Transform& GetTransform();
	// transformの設定
	void SetTransform(const Transform& trans);

private:

	struct MultiTexture
	{
		TextureResorce texResource;	// 通常テクスチャ
		TextureResorce sphResource;	// sphテクスチャ
		TextureResorce spaResource;	// spaテクスチャ
		TextureResorce subResource;	// サブテクスチャ
		TextureResorce toonResource;	// toonテクスチャ
	};

	struct MaterialForBuffer
	{
		DirectX::XMFLOAT4 diffuse;//拡散反射
		DirectX::XMFLOAT4 specular;//鏡面反射
		DirectX::XMFLOAT4 ambient;//環境光成分
		float power;	// スペキュラ乗数
	};

	ModelRenderer& _renderer;
	Dx12Wrapper& _dx12;


	std::shared_ptr<ModelData> _modelData;

	// 頂点バッファ
	ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};


	// 頂点インデックスバッファ
	ComPtr<ID3D12Resource> _indexBuffer = nullptr;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW _ibView = {};


	// テクスチャリソース
	std::vector<MultiTexture> _textures;

	// マテリアルバッファ
	ComPtr<ID3D12Resource> _materialBuffer = nullptr;
	ComPtr<ID3D12DescriptorHeap> _materialHeap = nullptr;

	Transform _trans;
	DirectX::XMMATRIX* _mappedTrans;

	// 座標行列用定数バッファ
	ComPtr<ID3D12Resource> _transCB = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> _worldHeap = nullptr;


	struct BoneNode
	{
		int boneIdx = 0;	// _boneMatsのインデックス
		DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの始点
		DirectX::XMFLOAT3 endPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの終点
		std::vector<BoneNode*> children; // 子供たちへのリンク
	};

	std::vector<DirectX::XMMATRIX> _boneMats;	// 各ボーンのtransform情報を格納
	std::unordered_map<std::wstring, BoneNode> _boneMap;	// ボーン名からboneMatsのインデックスをとる

	ComPtr<ID3D12Resource> _boneCB = nullptr;
	DirectX::XMMATRIX* _mappedBones;

	//vmd
	VMDMotion& _vmdMotion;

	uint32_t _lastTickTime;

	// 頂点bufferの作成
	bool CreateVertexBuffer();
	// 頂点インデックスバッファの作成
	bool CreateIndexBuffer();

	// マテリアルバッファの作成
	bool CreateMaterialBuffer();
	// マテリアルのテクスチャバッファ作成作成
	bool CreateMaterialTextureBuffer();
	// マテリアルのCBV作成
	bool CreateMaterialCBV();
	// マテリアルのテクスチャビューの作成
	void CreateMaterialTextureView(D3D12_CONSTANT_BUFFER_VIEW_DESC& viewDesc, D3D12_CPU_DESCRIPTOR_HANDLE& handle, const UINT& heapStride, unsigned int bufferStride, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

	// ボーン階層の作成
	bool CreateBoneHierarchy();

	// 行列計算の再帰関数
	void RecursiveMatrixMultiply(BoneNode& node, DirectX::XMMATRIX& inMat);

	// ボーン回転用関数
	void RotateBone(std::wstring boneName, DirectX::XMVECTOR location, DirectX::XMFLOAT4& q1, DirectX::XMFLOAT4& q2, float t);

	// 座標とボーン用のバッファとビュー作成
	bool CreateConstanteBuffers();

	// アニメーションの更新
	void MotionUpdate(const unsigned int motionFrame);
};