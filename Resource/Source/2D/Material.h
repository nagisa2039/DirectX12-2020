#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"

/// <summary>
/// マテリアルクラス(現状2Dペラ用)
/// </summary>
class Material
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="shaderPaht">シェーダーファイルパス</param>
	Material(const std::wstring& shaderPaht);
	~Material();

	const std::wstring& GetShaderPath()const;

	/// <summary>
	/// 個々のデスクリプターヒープの設定
	/// </summary>
	void SetEachDescriptorHeap(ID3D12GraphicsCommandList& cmdList);

private:
	const std::wstring shaderPath_;
	std::vector<int> textureIndexVec_;
	std::vector<float> constantFloatVec_;

	Resource texIndexResource;
	int* mappedTexIndex;
	ComPtr<ID3D12DescriptorHeap> textureIndexHeap_;
	Resource constantFloatResource;
	float* mappedConstantFloat;
	ComPtr<ID3D12DescriptorHeap> constantFloatHeap_;

	void CreateEachDataBuffer();

	/// <summary>
	/// 個々のデータを作成
	/// </summary>
	virtual void AddEachData() = 0;
};

