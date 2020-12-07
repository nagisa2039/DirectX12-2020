#pragma once
#include "3D/Mesh.h"
#include <vector>
#include <DirectXMath.h>
#include <string>
#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include <memory>
#include "Utility/TextureStruct.h"
#include "Utility/TimeLine.h"
#include "SkeletalMeshData.h"

class VMDMotion;
class ModelMaterial;
using Microsoft::WRL::ComPtr;

/// <summary>
/// ボーン有モデル
/// </summary>
class SkeletalMesh:
	public Mesh
{
public:
	/// <param name="owner">所有者</param>
	/// <param name="dx12">DirectX管理クラス</param>
	/// <param name="modelPath">モデルファイルパス</param>
	/// <param name="motionPath">モーションファイルパス</param>
	SkeletalMesh(std::weak_ptr<Actor> owner,
		Dx12Wrapper& dx12, const std::wstring& modelPath, const std::wstring& motionPath);
	~SkeletalMesh();

	// 初期化(モデルのファイルパス)
	bool Init(std::wstring modelPath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// Computeによる更新
	/// Rendererから呼ぶためにUpdateから分離
	/// </summary>
	void ComputeUpdate()override;

	/// <summary>
	/// 通常描画
	/// </summary>
	void Draw()override;
	
	/// <summary>
	/// アニメーションの開始
	/// </summary>
	void StartAnimation();

private:
	SkeletalMeshData& modelData_;
	ComPtr<ID3D12DescriptorHeap> heap_ = nullptr;

	// 頂点バッファ
	ResourceBindHeap vertexBufferUA_ = {};
	ResourceBindHeap vertexBufferSB_ = {};
	SkeletalMeshVertex* mappedVertexCB_ = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// 頂点インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// マテリアル
	std::unique_ptr<ModelMaterial> modelMaterial_;

	struct BoneNode
	{
		int boneIdx = 0;	// _boneMatsのインデックス
		DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの始点
		DirectX::XMFLOAT3 endPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ボーンの終点
		std::vector<BoneNode*> children; // 子供たちへのリンク
	};

	std::vector<DirectX::XMMATRIX> boneMats_;	// 各ボーンのtransform情報を格納
	std::unordered_map<std::wstring, BoneNode> boneMap_;	// ボーン名からboneMatsのインデックスをとる

	ResourceBindHeap boneCB_ = {};
	DirectX::XMMATRIX* mappedBones_ = nullptr;

	//vmd
	VMDMotion& vmdMotion_;

	uint64_t lastTickTime_;

	std::unique_ptr<TimeLine<float>> noiseThresholdTL_;

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
	bool CreateBoneBuffer();

	// アニメーションの更新
	void MotionUpdate(const unsigned int motionFrame);
};