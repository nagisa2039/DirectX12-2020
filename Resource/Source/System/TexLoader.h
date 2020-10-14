#pragma once
#include <functional>
#include <unordered_map>
#include <map>
#include <array>
#include "TextureStruct.h"

class IDXGISwapChain4;
class Dx12Wrapper;

class TexLoader
{
public:
	TexLoader(ID3D12Device& dev, Command& cmd, IDXGISwapChain4& swapChain);
	~TexLoader();

	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	const DummyTextures& GetDummyTextures()const;
	TextureResorce& GetTextureResouse(const int handle);
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);

	int LoadGraph(const std::string& path);

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

	TexLoader(const TexLoader&) = delete;
	TexLoader& operator=(const TexLoader&) = delete;

	bool Init();

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
	Resource _depthResouerce;
	// シャドウマップ用深度バッファ
	Resource _lightDepthResource;

	ComPtr<ID3D12DescriptorHeap> _depthDSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _depthSRVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _lightDSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _lightSRVHeap = nullptr;

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

	void CreateSRV(TextureResorce& texRes);
	bool CreateSwapChainBuffer(IDXGISwapChain4& swapChain);
};