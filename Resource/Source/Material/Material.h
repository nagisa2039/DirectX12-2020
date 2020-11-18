#pragma once
#include <string>
#include <vector>
#include "Utility/DirectXStruct.h"
#include "MaterialBase.h"

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
	/// マテリアル配列の取得
	/// </summary>
	std::vector <MaterialBase>& GetMaterialBaseVec();

	/// <summary>
	/// 追加テクスチャインデックス配列の取得
	/// </summary>
	std::vector<int>& GetAddTextureIndexVec();

	/// <summary>
	/// float定数配列の取得
	/// </summary>
	std::vector<float>& GetConstFloatVec();

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
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
	};

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				texIndexResource_;
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