#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include "Utility/DirectXStruct.h"
#include <DirectXTex.h>
#include <unordered_map>
#include "Shader/Struct/UtilityShaderStruct.h"

class Dx12Wrapper;
class Material;

/// <summary>
/// ブレンドモード
/// </summary>
enum class BlendMode
{
	noblend,	// ノーブレンド
	alpha,		// αブレンド
	add,		// 加算ブレンド
	sub,		// 減算ブレンド
	mula,		// 乗算ブレンド
	inv,		// 反転ブレンド
	max			// 選んではいけない
};

/// <summary>
/// 2D描画
/// </summary>
class SpriteDrawer
{
public:
	/// <summary>
	/// DX12管理クラス
	/// </summary>
	/// <param name="dx12"></param>
	SpriteDrawer(Dx12Wrapper& dx12);
	~SpriteDrawer();

	/// <summary>
	/// 描画時に使用するマテリアルの設定
	/// </summary>
	/// <param name="material">マテリアル</param>
	/// <returns>成否</returns>
	bool SetMaterial(std::shared_ptr<Material> material);

	/// <summary>
	/// 画像ハンドルを指定座標左上描画
	/// </summary>
	/// <param name="x">描画左座標</param>
	/// <param name="y">描画上座標</param>
	/// <param name="graphHandle">描画画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawGraph(const INT x, const INT y, const int graphHandle);

	/// <summary>
	/// 画像ハンドルを中心回転描画
	/// 描画座標は中心
	/// </summary>
	/// <param name="x">中心X座標</param>
	/// <param name="y">中心Y座標</param>
	/// <param name="exRate">拡大率</param>
	/// <param name="angle">回転(ラジアン)</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// 回転描画
	/// 描画座標は中心
	/// 回転位置を中心とは別に指定可能
	/// </summary>
	/// <param name="x">描画中心X座標</param>
	/// <param name="y">描画中心Y座標</param>
	/// <param name="centerX">回転中心X座標</param>
	/// <param name="centerY">回転中心Y座標</param>
	/// <param name="exRate">拡大率</param>
	/// <param name="angle">回転(ラジアン)</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// 画像切り抜き描画
	/// graphHから矩形部分を描画する
	/// </summary>
	/// <param name="destX">描画左座標</param>
	/// <param name="destY">描画上座標</param>
	/// <param name="srcX">切り抜き矩形の左座標</param>
	/// <param name="srcY">切り抜き矩形の上座標</param>
	/// <param name="width">切り抜き矩形の幅</param>
	/// <param name="height">切り抜き矩形の高さ</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	
	/// <summary>
	/// 画像切り抜き回転描画
	/// </summary>
	/// <param name="x">描画中心X座標</param>
	/// <param name="y">描画中心Y座標</param>
	/// <param name="srcX">切り抜き矩形の左座標</param>
	/// <param name="srcY">切り抜き矩形の上座標</param>
	/// <param name="width">切り抜き矩形の幅</param>
	/// <param name="height">切り抜き矩形の高さ</param>
	/// <param name="cx">回転中心X座標</param>
	/// <param name="cy">回転中心Y座標</param>
	/// <param name="exRate">拡大率</param>
	/// <param name="angle">回転(ラジアン)</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawRectRotaGraph2(const INT x, const INT y, const INT srcX, const INT srcY, const INT width, const INT height, const INT cx, const INT cy, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// 画像拡大描画
	/// 左上右下指定
	/// </summary>
	/// <param name="left">描画左座標</param>
	/// <param name="top">描画上座標</param>
	/// <param name="right">描画右座標</param>
	/// <param name="buttom">描画下座標</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle);
	
	/// <summary>
	/// 画像切り抜き拡大描画
	/// </summary>
	/// <param name="left">描画左座標</param>
	/// <param name="top">描画上座標</param>
	/// <param name="right">描画右座標</param>
	/// <param name="buttom">描画下座標</param>
	/// <param name="srcX">切り抜き矩形の左座標</param>
	/// <param name="srcY">切り抜き矩形の上座標</param>
	/// <param name="width">切り抜き矩形の幅</param>
	/// <param name="height">切り抜き矩形の高さ</param>
	/// <param name="graphHandle">画像ハンドル</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	
	/// <summary>
	/// 描画命令を閉じる
	/// 実行する
	/// </summary>
	void End();

	/// <summary>
	/// 描画時の明るさ設定(0～255)
	/// </summary>
	/// <param name="r">R成分</param>
	/// <param name="g">G成分</param>
	/// <param name="b">B成分</param>
	void SetDrawBright(const INT r, const INT g, const INT b);

	/// <summary>
	/// ブレンドモードモードの変更
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="value">ブレンド値</param>
	void SetDrawBlendMode(const BlendMode blendMode, const INT value);

	/// <summary>
	/// 描画先のスクリーンをセットする
	/// </summary>
	/// <param name="graphHandle">ｽｸﾘｰﾝ画像ハンドル</param>
	void SetDrawScreen(const int graphHandle);

private:
	SpriteDrawer(const SpriteDrawer&) = delete;
	SpriteDrawer& operator=(const SpriteDrawer&) = delete;

	Dx12Wrapper& dx12_;

	struct VerticesInf
	{
		DirectX::XMMATRIX posTans;
		DirectX::XMMATRIX uvTrans;
	};

	struct VertexResource
	{
		Resource resource;
		VerticesInf* mappedVertexInf = nullptr;
	};

	struct PixelInf
	{
		unsigned int texIndex = 0;
		DirectX::XMFLOAT3 bright = {};
		float alpha = 1.0f;
	};

	struct PixelInfResource
	{
		Resource resource;
		PixelInf* mappedPixelInf = nullptr;
	};

	struct DrawImage
	{
		VerticesInf verticesInf = {};
		PixelInf pixelInf = {};
	};

	Resource vertResource_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	Resource indexResource_;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	std::array<std::shared_ptr<Material>, static_cast<size_t>(BlendMode::max)> standeredBlendPipelineStates_;

	VertexResource verticesInfSB_;
	ComPtr<ID3D12DescriptorHeap> verticesInfHeap_ = nullptr;

	PixelInfResource pixelInfSB_;
	ComPtr<ID3D12DescriptorHeap> pixelInfHeap_ = nullptr;

	std::shared_ptr<Material> material_;
	DirectX::XMFLOAT3 drawBright_;
	float blendValue_;

	struct DrawGroup
	{
		std::shared_ptr<Material> material;
		int num = 1;
	};
	std::vector<DrawGroup> drawGroups_;
	std::vector<DrawImage> drawImages_;

	ComPtr<ID3DBlob> vertexShader_ = nullptr;
	ComPtr<ID3DBlob> pixelShader_ = nullptr;

	/// <summary>
	/// 自作ピクセルシェーダーごとのパイプラインマップ
	/// </summary>
	std::unordered_map<std::wstring, std::shared_ptr<Material>> pipelineStateMap_;

	struct UtilityResource
	{
		Resource resource;
		Utility* mapped = nullptr;
	};

	// シェーダーに送る定数
	UtilityResource utility_;
	ComPtr<ID3D12DescriptorHeap> utilityHeap_ = nullptr;

	void CreatePiplineState();
	void CreateRootSignature();
	void CreateVertextBuffer();

	void CreateIndexBuffer();

	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate = 1.0f, const float angle = 0.0f);
	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate = 1.0f, const float angle = 0.0f);
	void SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img);
	void AddDrawImage(SpriteDrawer::DrawImage& drawImage);

	void ClearDrawData();

	void GetDefaultInputElementDesc(std::vector<D3D12_INPUT_ELEMENT_DESC>& ied);
	void GetDefaultPipelineStateDesc(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied, D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd);
};

