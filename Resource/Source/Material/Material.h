#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"
#include "Shader/Struct/MaterialBase.h"

/// <summary>
/// マテリアルクラス(現状2Dペラ用)
/// </summary>
class Material
{
public:
	~Material();

	/// <summary>
	/// シェーダーファイルバスの取得
	/// </summary>
	const std::wstring& GetShaderPath()const;

	/// <summary>
	/// 個々のデスクリプターヒープの設定
	/// </summary>
	void SetEachDescriptorHeap(ID3D12GraphicsCommandList& cmdList);

	/// <summary>
	/// パイプラインステートの取得
	/// </summary>
	ComPtr<ID3D12PipelineState>& GetPipelineState();

	/// <summary>
	/// マテリアルベースの取得
	/// </summary>
	/// <param name="index">配列インデックス</param>
	const MaterialBase& GetMaterialBase(const size_t index)const;

	/// <summary>
	/// マテリアルベースの設定
	/// </summary>
	/// <param name="index">配列インデックス</param>
	/// <param name="value">設定する値</param>
	void SetMaterialBase(const size_t index, const MaterialBase& value);
	
	/// <summary>
	/// 追加テクスチャインデックスの設定
	/// </summary>
	/// <param name="index">配列インデックス</param>
	/// <param name="value">設定する値</param>
	void SetAddTexIndex	(const size_t index, const int& value);

	/// <summary>
	/// 定数floatの設定
	/// </summary>
	/// <param name="index">配列インデックス</param>
	/// <param name="value">設定する値</param>
	void SetConstFloat	(const size_t index, const float& value);

protected:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="shaderPaht">シェーダーファイルパス</param>
	Material(const std::wstring& shaderPaht);

	template<class T>
	struct StructuredResource
	{
		Resource resource = {};
		std::vector<T> elements;
		T* mapped = nullptr;
		D3D12_GPU_DESCRIPTOR_HANDLE handle = {};
	};

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				addTexIndexResource_;
	StructuredResource<float>			constFloatResource_;

	/// <summary>
	/// データが決まったらバッファとヒープを作成する
	/// </summary>
	void CreateEachDataBuffer();

private:
	const std::wstring shaderPath_;
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> heap_ = nullptr;

};