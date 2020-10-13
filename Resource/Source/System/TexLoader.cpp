#include "TexLoader.h"
#include "d3dx12.h"
#include <cassert>
#include <dxgi1_6.h>
#include "Command.h"
#include "Application.h"
#include "Utility/dx12Tool.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

namespace
{
	constexpr uint32_t shadow_resolution = 2048;
}


TexLoader::TexLoader(ID3D12Device& dev, Command& cmd): _dev(dev), _cmd(cmd)
{
	Init();
}


TexLoader::~TexLoader()
{
}

bool TexLoader::Init()
{
	// 白テクスチャの作成
	if (!CreateScreenBuffer(_dummyTextures.whiteTex, 4, 4 ,255))
	{
		assert(false);
		return false;
	}
	// 黒テクスチャの作成
	if (!CreateScreenBuffer(_dummyTextures.blackTex, 4, 4, 255))
	{
		assert(false);
		return false;
	}
	// グラデーションテクスチャの作成
	if (!CreateGradTexture(_dummyTextures.gradTex))
	{
		assert(false);
		return false;
	}

	// 画像読込関数テーブルの作成
	if (!CretateLoadLambdaTable())
	{
		assert(false);
		return false;
	}

	// テクスチャ用デスクリプタヒープの作成
	CreateTextureHeap();

	// 深度バッファの作成
	CreateDepthBuffer();

	return true;
}

const ComPtr<ID3D12DescriptorHeap>& TexLoader::GetTextureHeap() const
{
	return _texHeap;
}

const DummyTextures & TexLoader::GetDummyTextures()const
{
	return _dummyTextures;
}

TextureResorce& TexLoader::GetTextureResouse(const int handle)
{
	if (handle < 0 || handle >= _texResources.size())
	{
		assert(false);
	}

	return _texResources[handle];
}

bool TexLoader::GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes)
{
	// リソーステールにあったらそれを返す
	auto it = _resouseHandleTable.find(texPath);
	if (it != _resouseHandleTable.end())
	{
		texRes = _texResources[(*it).second];
		return true;
	}

	if (!LoadPictureFromFile(texPath, texRes))
	{
		return false;
	}
	return true;
}

bool TexLoader::CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const int color)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;// テクスチャ用
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// ランダムカラー画像の作成
	const UINT pixelSize = 4;
	vector<uint8_t> col(width * height * pixelSize);
	fill(col.begin(), col.end(), color);
	resource.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	D3D12_CLEAR_VALUE clearValue = { DXGI_FORMAT_R8G8B8A8_UNORM , { 0.0f, 0.0f, 0.0f, 1.0f } };
	H_ASSERT(_dev.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resDesc,
		resource.state,
		&clearValue,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	// GPUに書き込む
	H_ASSERT(resource.buffer->WriteToSubresource(0,
		nullptr,	// 全領域コピー
		col.data(),	// 元データのアドレス
		width * pixelSize,	// 1ラインのサイズ
		static_cast<UINT>(col.size()// 1枚のサイズ
	)));

	return true;
}

bool TexLoader::CreateGradTexture(Resource& resource)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;// テクスチャ用
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 4, 256);

	struct Color
	{
		uint8_t r, g, b, a;
		Color(uint8_t inr, uint8_t ing, uint8_t inb, uint8_t ina) :
			r(inr), g(ing), b(inb), a(ina) {};
		Color() {};
	};

	unsigned int redWidthUint = static_cast<unsigned int>(resDesc.Width);
	vector<Color> col(redWidthUint * 256);
	auto it = col.begin();
	for (int j = 255; j >= 0; j--)
	{
		uint8_t u8j = static_cast<uint8_t>(j);
		fill(it, it + redWidthUint, Color(u8j, u8j, u8j, 255));
		it += redWidthUint;
	}

	H_ASSERT(_dev.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	// GPUに書き込む
	H_ASSERT(resource.buffer->WriteToSubresource(0,
		nullptr,	// 全領域コピー
		col.data(),	// 元データのアドレス
		static_cast<UINT>(sizeof(Color)* resDesc.Width),	// 1ラインのサイズ
		static_cast<UINT>(sizeof(Color)* col.size()	// 1枚のサイズ
	)));

	return true;
}

bool TexLoader::CretateLoadLambdaTable()
{
	_loadLambdaTable[L"bmp"] = _loadLambdaTable[L"sph"] = _loadLambdaTable[L"spa"]
		= _loadLambdaTable[L"png"] = _loadLambdaTable[L"jpg"] = _loadLambdaTable[L"jpeg"] =
		[](const wstring& path, TexMetadata* meta, ScratchImage&img)->HRESULT
	{
		return LoadFromWICFile(path.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, meta, img);
	};
	_loadLambdaTable[L"tga"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};
	_loadLambdaTable[L"dds"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT
	{
		return LoadFromDDSFile(path.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, meta, img);
	};

	return true;
}

bool TexLoader::CreateTextureFromImageData(const ScratchImage& scrachImage, Resource& resource, const TexMetadata& metaData, bool isDiscrete)
{
	auto img = scrachImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES heapProp = {};
	if (isDiscrete)
	{
		// グラボが離れているの場合
		// あとから作るUPLOADBuffaを中間バッファとして用いり
		// CopyTecureRegionで転送する
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	}
	else
	{
		//WriteToSubresouse方式
		// グラボが一体型の場合はCUSTOM
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	}

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.Width = img->width;	// リソースのサイズX
	resDesc.Height = static_cast<UINT>(img->height);	// リソースのサイズY
	resDesc.Format = metaData.format;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);
	resDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);
	resDesc.SampleDesc.Quality = 0;
	resDesc.SampleDesc.Count = 1;

	resource.state = isDiscrete ?
		D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	H_ASSERT(_dev.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		resource.state,
		nullptr,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	return true;
}

// 画像の読み込み
bool TexLoader::LoadPictureFromFile(const std::wstring& texPath, TextureResorce& texRes)
{
	// 画像読み込み
	TexMetadata metaData = {};
	ScratchImage scratchImg = {};

	int idx = static_cast<int>(texPath.find_last_of(L'.'));
	auto ext = texPath.substr(idx + 1, texPath.length() - idx - 1);
	if (FAILED(_loadLambdaTable[ext](texPath, &metaData, scratchImg)))
	{
		//assert(false);
		return false;
	}

	auto img = scratchImg.GetImage(0, 0, 0);	// 生データの抽出
	texRes.imageInf = *img;

	// 読み込んだ情報をもとにテクスチャの作成
	if (!CreateTextureFromImageData(scratchImg, texRes.resource, metaData, true))
	{
		//assert(false);
		return false;
	}

	// 転送のための中間バッファの作成
	auto imgRowPitch = AlignmentValue(static_cast<UINT>(img->rowPitch), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(imgRowPitch * img->height);

	ComPtr<ID3D12Resource> uploadbuff = nullptr;
	H_ASSERT(_dev.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// cpuからの書き込み可能
		nullptr,
		IID_PPV_ARGS(uploadbuff.ReleaseAndGetAddressOf())));

	uint8_t* mapforImg = nullptr;//image->pixelsと同じ型にする
	uploadbuff->Map(0, nullptr, (void**)&mapforImg);//マップ
	auto address = img->pixels;
	for (unsigned int j = 0; j < img->height; j++)
	{
		copy_n(address, img->rowPitch, mapforImg);
		address += img->rowPitch;
		mapforImg += imgRowPitch;
	}
	//copy_n(img->pixels, img->slicePitch, mapforImg);//コピー
	uploadbuff->Unmap(0, nullptr);//アンマップ


	D3D12_TEXTURE_COPY_LOCATION src = {}, dst = {};

	// コピー元
	src.pResource = uploadbuff.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData.depth);
	src.PlacedFootprint.Footprint.RowPitch = imgRowPitch;
	src.PlacedFootprint.Footprint.Format = img->format;


	// コピー先
	dst.pResource = texRes.resource.buffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// コピー命令
	auto& cmdList = _cmd.CommandList();
	cmdList.CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// バリアの設定
	texRes.resource.Barrier(_cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	_cmd.Execute();

	return true;
}

int TexLoader::LoadTexture(const std::string& path)
{
	auto wstringPath = WStringFromString(path);
	if (_resouseHandleTable.find(wstringPath) != _resouseHandleTable.end())
	{
		return _resouseHandleTable[wstringPath];
	}

	TextureResorce texRes;
	if (!GetTextureResouse(wstringPath, texRes))
	{
		return -1;
	}

	// ResourceをもとSRVをに作成
	CreateSRV(texRes);

	_texResources.emplace_back(texRes);
	_resouseHandleTable[wstringPath] = _texResources.size() - 1;

	return _resouseHandleTable[wstringPath];
}

void TexLoader::CreateSRV(TextureResorce& texRes)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Format = texRes.resource.buffer->GetDesc().Format;

	auto incSize = _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto cpuHandle = _texHeap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += _texResources.size() * incSize;
	_dev.CreateShaderResourceView(texRes.resource.buffer.Get(), &srvDesc, cpuHandle);

	texRes.cpuHandleForTex = cpuHandle;
	texRes.gpuHandleForTex = _texHeap->GetGPUDescriptorHandleForHeapStart();
	texRes.gpuHandleForTex.ptr += _texResources.size() * incSize;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texRes.resource.buffer->GetDesc().Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	incSize = _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	cpuHandle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += _texResources.size() * incSize;
	_dev.CreateRenderTargetView(texRes.resource.buffer.Get(), &rtvDesc, cpuHandle);
	texRes.cpuHandleForRtv = cpuHandle;
	texRes.gpuHandleForRtv = _rtvHeap->GetGPUDescriptorHandleForHeapStart();
	texRes.gpuHandleForRtv.ptr += _texResources.size() * incSize;
}

bool TexLoader::CreateSwapChainBuffer(IDXGISwapChain4& swapChain)
{
	DXGI_SWAP_CHAIN_DESC1 swDesc = {};
	swapChain.GetDesc1(&swDesc);

	for (size_t i = 0; i < swDesc.BufferCount; i++)
	{
		TextureResorce texRes;
		H_ASSERT(swapChain.GetBuffer(i, IID_PPV_ARGS(texRes.resource.buffer.ReleaseAndGetAddressOf())));
		texRes.resource.state	= D3D12_RESOURCE_STATE_PRESENT;
		texRes.imageInf.width	= swDesc.Width;
		texRes.imageInf.height	= swDesc.Height;
		CreateSRV(texRes);
		_texResources.emplace_back(texRes);
	}

	return true;
}

void TexLoader::ClsDrawScreen()
{
	assert(_renderTergetHandle >= 0 && _renderTergetHandle < _texResources.size());

	auto& commandList = _cmd.CommandList();

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList.ClearRenderTargetView(_texResources[_renderTergetHandle].cpuHandleForRtv, clearColor, 0, nullptr);

	// 深度バッファを初期化
	commandList.ClearDepthStencilView(
		_depthDSVHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void TexLoader::SetDrawScreen(const int screenH)
{
	if (screenH != _renderTergetHandle)
	{
		if (_renderTergetHandle >= 0)
		{
			// 今までのレンダーターゲットのステートをPIXEL_SHADER_RESOURCEにする
			_texResources[_renderTergetHandle].resource.Barrier(_cmd, _renderTergetHandle >= 2 ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_PRESENT);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* depthH = &_depthDSVHeap->GetCPUDescriptorHandleForHeapStart();

		// セットするレンダーターゲットのステートをRENDER_TARGEにする
		_texResources[screenH].resource.Barrier(_cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmd.CommandList().OMSetRenderTargets(1, &_texResources[screenH].cpuHandleForRtv, false, depthH);
		_renderTergetHandle = screenH;
	}
}

void TexLoader::ScreenFlip(IDXGISwapChain4& swapChain)
{
	// swapChainのダブルバッファ以外が指定されていたら例外
	assert(_renderTergetHandle < 2);

	// レンダーターゲットをプレゼント用にバリアを張る
	auto& texRes = GetTextureResouse(_renderTergetHandle);
	assert(texRes.resource.state == D3D12_RESOURCE_STATE_RENDER_TARGET);
	texRes.resource.Barrier(_cmd, D3D12_RESOURCE_STATE_PRESENT);

	_cmd.Execute();

	// 裏画面と表画面の切り替え
	swapChain.Present(1, 0);

}

int TexLoader::MakeScreen(const UINT width, const UINT height)
{
	TextureResorce texRes = {};
	CreateScreenBuffer(texRes.resource, width, height);
	texRes.imageInf.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texRes.imageInf.width = width;
	texRes.imageInf.height = height;

	CreateSRV(texRes);

	_texResources.emplace_back(texRes);
	return _texResources.size() - 1;
}

int TexLoader::GetCurrentRenderTarget() const
{
	return _renderTergetHandle;
}

bool TexLoader::GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const
{
	if (graphH < 0 || graphH >= _texResources.size())
	{
		assert(false);
		return false;
	}

	width	= _texResources[graphH].imageInf.width;
	height	= _texResources[graphH].imageInf.height;
	return true;
}

bool TexLoader::GetScreenSize(unsigned int& width, unsigned int& height) const
{
	return GetGraphSize(GetCurrentRenderTarget(), width, height);
}

void TexLoader::CreateTextureHeap()
{
	const auto imageMax = Application::Instance().GetImageMax();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = imageMax;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	H_ASSERT(_dev.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_texHeap.ReleaseAndGetAddressOf())));

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	H_ASSERT(_dev.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_rtvHeap.ReleaseAndGetAddressOf())));
}

bool Resource::Barrier(Command& cmd, const D3D12_RESOURCE_STATES changeState)
{
	if (state == changeState) return false;

	cmd.CommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), state, changeState));
	state = changeState;
	return true;
}


bool TexLoader::CreateDepthBuffer()
{
	// 深度バッファの作成
	auto wsize = Application::Instance().GetWindowSize();

	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, wsize.w, wsize.h);
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// 重要らしい
	D3D12_CLEAR_VALUE depthClearValue = {};
	// 深さの最大値1にする
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	_depthBuffer.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	

	if (FAILED(_dev.CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		_depthBuffer.state,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// シャドウマップ用
	depthResDesc.Width = shadow_resolution;
	depthResDesc.Height = shadow_resolution;
	_lightDepthBuffer.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (FAILED(_dev.CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		_lightDepthBuffer.state,
		&depthClearValue,
		IID_PPV_ARGS(_lightDepthBuffer.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// 深度ステンシルビューの作成
	if (!CreateDSVAndSRV())
	{
		return false;
	}

	return true;
}

bool TexLoader::CreateDSVAndSRV()
{
	// 深度バッファビューの作成
	// デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;		// 0 は描画深度 1 はライト深度
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(_dev.CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(_depthDSVHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	auto DSVhandle = _depthDSVHeap->GetCPUDescriptorHandleForHeapStart();

	// 描画用
	_dev.CreateDepthStencilView(
		_depthBuffer.buffer.Get(),
		&dsvDesc,
		DSVhandle);
	DSVhandle.ptr += _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//// シャドウマップ用
	//_dev.CreateDepthStencilView(
	//	_lightDepthBuffer.buffer.Get(),
	//	&dsvDesc,
	//	DSVhandle);

	//  SR用デスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2;		// 0 は描画深度 1 はライト深度
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	if (FAILED(_dev.CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(_depthSRVHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// SR用のビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	auto SRVhandle = _depthSRVHeap->GetCPUDescriptorHandleForHeapStart();

	// 描画用
	_dev.CreateShaderResourceView(
		_depthBuffer.buffer.Get(),
		&srvDesc,
		SRVhandle);
	SRVhandle.ptr += _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//// シャドウマップ用
	//_dev.CreateShaderResourceView(
	//	_lightDepthBuffer.buffer.Get(),
	//	&srvDesc,
	//	SRVhandle);

	return true;
}