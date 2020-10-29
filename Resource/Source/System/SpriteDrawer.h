#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <array>
#include <DirectXMath.h>
//#include<SpriteFont.h>
//#include<ResourceUploadBatch.h>
#include "Utility/DirectXStruct.h"
#include <DirectXTex.h>

class Dx12Wrapper;

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

class SpriteDrawer
{
public:

	SpriteDrawer(Dx12Wrapper& dx12);
	~SpriteDrawer();

	bool DrawGraph(const INT x, const INT y, const int graphHandle);
	bool DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle);
	bool DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle);
	bool DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	bool DrawRectRotaGraph2(const INT x, const INT y, const INT srcX, const INT srcY, const INT width, const INT height, const INT cx, const INT cy, const float exRate, const float angle, const int graphHandle);
	bool DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle);
	bool DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	bool DrawModiGraph(const INT x1, const INT y1, const INT x2, const INT y2, const INT x3, const INT y3, const INT x4, const INT y4, const int GrHandle, const int TransFlag);
	void End();

	void SetDrawBright(const INT r, const INT g, const INT b);
	void SetDrawBlendMode(const BlendMode blendMode, const INT value);

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
		DirectX::XMFLOAT3 bright;
		float alpha = 1.0f;
	};

	struct PixelInfResource
	{
		Resource resource;
		PixelInf* mappedPixelInf = nullptr;
	};

	struct DrawImage
	{
		VerticesInf verticesInf;
		PixelInf pixelInf;
	};

	Resource vertResource_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	Resource indexResource_;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	std::array<ComPtr<ID3D12PipelineState>, static_cast<size_t>(BlendMode::max)> pipelineStates_;

	VertexResource verticesInfSB_;
	ComPtr<ID3D12DescriptorHeap> verticesInfHeap_ = nullptr;

	PixelInfResource pixelInfSB_;
	ComPtr<ID3D12DescriptorHeap> pixelInfHeap_ = nullptr;

	BlendMode blendMode_;
	DirectX::XMFLOAT3 drawBright_;
	float blendValue_;

	struct BlendGroup
	{
		BlendMode blendMode = BlendMode::noblend;
		int num = 1;
	};
	std::vector<BlendGroup> blendGroups_;
	std::vector<DrawImage> drawImages_;

	ComPtr<ID3DBlob> vertexShader_ = nullptr;
	ComPtr<ID3DBlob> pixelShader_ = nullptr;

	//ComPtr<ID3D12DescriptorHeap> spriteFontHeap_ = nullptr;
	//std::shared_ptr<DirectX::GraphicsMemory> gmemory_ = nullptr;//グラフィクスメモリオブジェクト
	//std::shared_ptr<DirectX::SpriteFont> spriteFont_ = nullptr;//フォント表示用オブジェクト
	//std::shared_ptr<DirectX::SpriteBatch> spriteBatch_ = nullptr;//スプライト表示用オブジェクト

	void CreateVertexSB();
	void CreatePixelSB();

	void CreatePiplineState();
	void CreateRootSignature();
	void CreateVertextBuffer();

	void CreateIndexBuffer();

	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate = 1.0f, const float angle = 0.0f);
	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate = 1.0f, const float angle = 0.0f);
	void SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img);
	void AddDrawImage(SpriteDrawer::DrawImage& drawImage);

	//void CreateSpriteHeap();

	void ClearDrawData();
};

