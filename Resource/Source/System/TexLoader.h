#pragma once
#include <functional>
#include <unordered_map>
#include <map>
#include <array>
#include "TextureStruct.h"

class IDXGISwapChain4;


class TexLoader
{
public:
	TexLoader(ID3D12Device& dev, Command& cmd);
	~TexLoader();

	bool Init();
	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	const DummyTextures& GetDummyTextures()const;
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);
	TextureResorce& GetTextureResouse(const int handle);

	int LoadTexture(const std::string& path);
	void CreateSRV(TextureResorce& texRes);

	bool CreateSwapChainBuffer(IDXGISwapChain4& swapChain);

	void ClsDrawScreen();
	void SetDrawScreen(const int screenH);
	void ScreenFlip(IDXGISwapChain4& swapChain);

	int MakeScreen(const UINT width, const UINT height);

	int GetCurrentRenderTarget()const;
	bool GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const;
	bool GetScreenSize(unsigned int& width, unsigned int& height)const;

private:

	ID3D12Device& _dev;
	Command& _cmd;

	// 画像ロード用lambda格納マップ
	using LoadLambda_t = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::unordered_map<std::wstring, LoadLambda_t> _loadLambdaTable;

	// テクスチャバッファ
	std::unordered_map<std::wstring, int> _resouseHandleTable;
	DummyTextures _dummyTextures;

	std::vector<TextureResorce> _texResources;
	ComPtr<ID3D12DescriptorHeap> _texHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr;

	int _renderTergetHandle;

	// 描画用深度バッファ
	Resource _depthBuffer;
	// シャドウマップ用深度バッファ
	Resource _lightDepthBuffer;

	ComPtr<ID3D12DescriptorHeap> _depthDSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _depthSRVHeap = nullptr;

	// スクリーン用バッファの生成　作成先バッファ	ｽｸﾘｰﾝの幅と高さ		初期化色
	bool CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const int color = 0);
	// グラデーションテクスチャの生成	(tonn が無いときに使う階調)
	bool CreateGradTexture(Resource& resource);

	// 画像ロードテーブルの作成
	bool CretateLoadLambdaTable();

	// シェーダーリソースの作成
	bool CreateTextureFromImageData(const DirectX::ScratchImage& scrachImage, Resource& resource, const DirectX::TexMetadata& metaData, bool isDiscread = false);

	// 画像読み込み
	bool LoadPictureFromFile(const std::wstring& texPath, TextureResorce& texRes);

	void CreateTextureHeap();

	// 深度バッファの作成
	bool CreateDepthBuffer();
	// 深度ステンシルビューの作成
	bool CreateDSVAndSRV();
};