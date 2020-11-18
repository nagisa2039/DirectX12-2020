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
#include "Utility/TextureStruct.h"

class VMDMotion;
class Dx12Wrapper;
class ModelRenderer;
class ModelMaterial;
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
	void Draw();
	// アニメーションを開始する
	void StartAnimation();
	// transform取得
	Transform& GetTransform();
	// transformの設定
	void SetTransform(const Transform& trans);

private:

	ModelRenderer& renderer_;
	Dx12Wrapper& dx12_;


	std::shared_ptr<ModelData> modelData_;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};


	// 頂点インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// マテリアル
	std::unique_ptr<ModelMaterial> modelMaterial_;

	// マテリアルインデックスバッファ
	ComPtr<ID3D12Resource> materialIndexBuffer_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> materialIndexHeap_ = nullptr;

	Transform trans_;
	DirectX::XMMATRIX* mappedTrans_;

	// 座標行列用定数バッファ
	ComPtr<ID3D12Resource> transCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> worldHeap_ = nullptr;

	struct BoneNode
	{
		int boneIdx = 0;	// _boneMatsのインデックス
		DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの始点
		DirectX::XMFLOAT3 endPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの終点
		std::vector<BoneNode*> children; // 子供たちへのリンク
	};

	std::vector<DirectX::XMMATRIX> boneMats_;	// 各ボーンのtransform情報を格納
	std::unordered_map<std::wstring, BoneNode> boneMap_;	// ボーン名からboneMatsのインデックスをとる

	ComPtr<ID3D12Resource> boneCB_ = nullptr;
	DirectX::XMMATRIX* mappedBones_;

	//vmd
	VMDMotion& vmdMotion_;

	uint64_t lastTickTime_;

	// 頂点bufferの作成
	bool CreateVertexBuffer();
	// 頂点インデックスバッファの作成
	bool CreateIndexBuffer();

	// マテリアルの作成
	bool CreateMaterial();

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