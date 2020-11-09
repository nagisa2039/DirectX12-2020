#pragma once
#include <functional>
#include <unordered_map>
#include <map>
#include <array>
#include "Utility/TextureStruct.h"

struct IDXGISwapChain4;
class Dx12Wrapper;

class TexLoader
{
public:
	TexLoader(ID3D12Device& dev, Command& cmd, IDXGISwapChain4& swapChain);
	~TexLoader();

	enum class DepthType
	{
		camera,
		light,
		max
	};

	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	const DummyTextures& GetDummyTextureHandles()const;
	TextureResorce& GetTextureResouse(const int handle);
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);

	int GetGraphHandle(const std::wstring& texPath)const;

	int LoadGraph(const std::wstring& path);

	void ClsDrawScreen();
	void SetDrawScreen(const int screenH, const DepthType depth = DepthType::max);
	void ScreenFlip(IDXGISwapChain4& swapChain);

	int MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height);

	int GetCurrentRenderTarget()const;
	bool GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const;
	bool GetScreenSize(unsigned int& width, unsigned int& height)const;

	void SetDepthTexDescriptorHeap(const UINT rootParamIdx, const DepthType depth);
	void SetTextureDescriptorHeap(const UINT rootParamIdx);

private:
	ID3D12Device& dev_;
	Command& cmd_;

	TexLoader(const TexLoader&) = delete;
	TexLoader& operator=(const TexLoader&) = delete;

	bool Init();

	bool CreateDummyTextures();

	// 画像ロード用lambda格納マップ
	using LoadLambda_t = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::unordered_map<std::wstring, LoadLambda_t> loadTable_;

	// テクスチャバッファ
	std::unordered_map<std::wstring, int> resourceHandleTable_;
	DummyTextures dummyTextures_;

	std::vector<TextureResorce> texResources_;
	ComPtr<ID3D12DescriptorHeap> texHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;

	int renderTergetHandle_;

	std::array<TextureResorce, static_cast<size_t>(DepthType::max)> depthTexResources_;
	DepthType currentDepthType_;

	ComPtr<ID3D12DescriptorHeap> depthDSVHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> depthSRVHeap_ = nullptr;

	int MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height, const std::vector<uint8_t>& colorData);

	// スクリーン用バッファの生成　作成先バッファ	ｽｸﾘｰﾝの幅と高さ		初期化色
	bool CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const std::vector<uint8_t>& colorData);
	
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