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
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="shaderPaht">シェーダーファイルパス</param>
	Material(const std::wstring& shaderPaht);
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

protected:
	MaterialBase& GetMaterialBase();
	std::vector<int>& GetAddTextureIndexVec();
	std::vector<float>& GetConstFloatVec();

	/// <summary>
	/// 個々のデータを作成
	/// </summary>
	//virtual void AddEachData() = 0;
	void CreateEachDataBuffer();

private:
	const std::wstring shaderPath_;

	template<class T>
	struct StructuredResource
	{
		Resource resource = {};
		std::vector<T> elements;
		T* mapped = nullptr;
		ComPtr<ID3D12DescriptorHeap> heap = nullptr;
	};

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	StructuredResource<MaterialBase>	materialBaseResource_;
	StructuredResource<int>				texIndexResource_;
	StructuredResource<float>			constFloatResource_;


};