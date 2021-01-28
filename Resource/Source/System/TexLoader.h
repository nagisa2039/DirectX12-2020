#pragma once
#include <functional>
#include <unordered_map>
#include <map>
#include <array>
#include <list>
#include "Utility/TextureStruct.h"

struct IDXGISwapChain4;
class Dx12Wrapper;

/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TexLoader
{
public:
	/// <param name="dev">デバイス</param>
	/// <param name="cmd">コマンド管理クラス</param>
	/// <param name="swapChain">スワップチェイン</param>
	TexLoader(ID3D12Device& dev, Command& cmd, IDXGISwapChain4& swapChain);
	~TexLoader();

	// 深度
	enum class DepthType
	{
		// カメラ
		camera,
		// ライト
		light,
		// 使用しない
		max
	};

	/// <summary>
	/// テクスチャ管理でスクリプタヒープの取得
	/// </summary>
	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	/// <summary>
	/// ダミーテクスチャハンドルたちの取得
	/// </summary>
	const DummyTextures& GetDummyTextureHandles()const;

	/// <summary>
	/// テクスチャリソースの取得
	/// </summary>
	/// <param name="handle">画像ハンドル</param>
	const TextureResorce& GetTextureResouse(const int handle)const;

	/// <summary>
	/// テクスチャリソースの取得
	/// </summary>
	/// <param name="texPath">テクスチャパス</param>
	/// <param name="texRes">テクスチャリソースの格納先</param>
	/// <returns>成否</returns>
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);

	/// <summary>
	/// 画像ハンドルの取得
	/// 未読込なら読み込む
	/// </summary>
	/// <param name="path">テクスチャパス</param>
	/// <returns>画像ハンドル</returns>
	int GetGraphHandle(const std::wstring& path);

	/// <summary>
	/// 現在設定されている描画先をクリア(リセット)する
	/// </summary>
	void ClsDrawScreen();

	/// <summary>
	/// 描画先の設定
	/// </summary>
	/// <param name="screenH">描画先画像ハンドル</param>
	/// <param name="depth">使用する深度</param>
	void SetDrawScreen(const int screenH, const DepthType depth = DepthType::max);
	
	/// <summary>
	/// 描画先の設定
	/// 複数枚設定
	/// </summary>
	/// <param name="screenHList">描画先画像ハンドルリスト</param>
	/// <param name="depth">使用する深度</param>
	void SetDrawScreen(const std::list<int>& screenHList, const DepthType depth = DepthType::max);
	
	/// <summary>
	/// SwapChainのプリップ
	/// </summary>
	/// <param name="swapChain"></param>
	void ScreenFlip(IDXGISwapChain4& swapChain);

	/// <summary>
	/// 描画先画像の作成
	/// </summary>
	/// <param name="resourceName">使用名</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <returns>画像ハンドル</returns>
	int MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height);

	/// <summary>
	/// 現在設定されているレンダーターゲットの一つ目のハンドルを返す
	/// </summary>
	int GetCurrentRenderTarget()const;

	/// <summary>
	/// 現在設定されているレンダーターゲットの配列取得
	/// </summary>
	std::list<int> GetCurrentRendetTargeAll()const;

	/// <summary>
	/// 画像サイズの取得
	/// </summary>
	/// <param name="graphH">画像ハンドル</param>
	/// <param name="width">幅格納変数</param>
	/// <param name="height">高さ格納変数</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const;
	
	/// <summary>
	/// 現在のレンダーターゲットサイズの取得
	/// </summary>
	/// <param name="width">幅格納変数</param>
	/// <param name="height">高さ格納変数</param>
	/// <returns>有効な画像ハンドルか</returns>
	bool GetScreenSize(unsigned int& width, unsigned int& height)const;

	/// <summary>
	/// 深度でスクリプタヒープの設定
	/// </summary>
	/// <param name="rootParamIdx">パラメータインデックス</param>
	/// <param name="depth">深度タイプ</param>
	void SetDepthTexDescriptorHeap(const UINT rootParamIdx, const DepthType depth);
	
	/// <summary>
	/// テクスチャ配列のデスクリプタヒープを設定
	/// </summary>
	/// <param name="rootParamIdx">パラメータインデックス</param>
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

	std::list<int> renderTergetHandleList_;

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

	void CreateView(TextureResorce& texRes);
	bool CreateSwapChainBuffer(IDXGISwapChain4& swapChain);

	/// <summary>
	/// 指定した画像ハンドルがあるか確認する
	/// </summary>
	/// <param name="texPath">読み込み時のパス</param>
	/// <returns>画像ハンドル</returns>
	int FindGraphHandle(const std::wstring& texPath)const;

	// テクスチャリソースが存在するか確認
	bool ExistTextureResource(const int handle)const;
};