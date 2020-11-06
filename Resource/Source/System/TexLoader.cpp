#include "TexLoader.h"
#include "d3dx12.h"
#include <cassert>
#include <dxgi1_6.h>
#include <algorithm>
#include "Command.h"
#include "Application.h"
#include "Utility/dx12Tool.h"
#include "Utility/Constant.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

namespace
{
	const uint8_t pixelSize = 4;
}

TexLoader::TexLoader(ID3D12Device& dev, Command& cmd, IDXGISwapChain4& swapChain)
	: dev_(dev), cmd_(cmd)
{
	Init();
	CreateSwapChainBuffer(swapChain);
	CreateDummyTextures();
}

TexLoader::~TexLoader()
{
}

bool TexLoader::Init()
{
	// �e�N�X�`���p�f�X�N���v�^�q�[�v�̍쐬
	CreateTextureHeap();

	// �摜�Ǎ��֐��e�[�u���̍쐬
	if (!CretateLoadLambdaTable())
	{
		assert(false);
		return false;
	}

	// �[�x�o�b�t�@�̍쐬
	CreateDepthBuffer();

	return true;
}

bool TexLoader::CreateDummyTextures()
{
	const UINT width = 4;
	const UINT height = 4;
	std::vector<uint8_t> colorData(width * height * pixelSize);
	std::fill(colorData.begin(), colorData.end(), 255);

	// ���e�N�X�`���̍쐬
	dummyTextures_.whiteTexH = MakeScreen(L"whiteTex", width, height, colorData);
	if (dummyTextures_.whiteTexH == FAILED)
	{
		assert(false);
		return false;
	}

	// ���e�N�X�`���̍쐬
	std::fill(colorData.begin(), colorData.end(), 0);
	dummyTextures_.blackTexH = MakeScreen(L"blackTex", width, height, colorData);
	if (dummyTextures_.blackTexH == FAILED)
	{
		assert(false);
		return false;
	}

	const int gradHeight = 256;
	colorData.resize(width * gradHeight * pixelSize);
	auto it = colorData.begin();
	const auto strideInBytes = width * pixelSize;
	for (int j = gradHeight - 1; j >= 0; j--)
	{
		uint8_t u8t = Uint8(j);
		std::fill(it, it + strideInBytes, u8t);
		it += strideInBytes;
	}
	dummyTextures_.gradTexH = MakeScreen(L"gradTex", width, gradHeight, colorData);
	if (dummyTextures_.gradTexH == FAILED)
	{
		assert(false);
		return false;
	}
	return true;
}

const ComPtr<ID3D12DescriptorHeap>& TexLoader::GetTextureHeap() const
{
	return texHeap_;
}

const DummyTextures & TexLoader::GetDummyTextureHandles()const
{
	return dummyTextures_;
}

TextureResorce& TexLoader::GetTextureResouse(const int handle)
{
	if (handle < 0 || handle >= texResources_.size())
	{
		assert(false);
	}

	return texResources_[handle];
}

bool TexLoader::GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes)
{
	// ���\�[�X�e�[���ɂ������炻���Ԃ�
	int handle = GetGraphHandle(texPath);
	if (handle != FAILED)
	{
		texRes = GetTextureResouse(handle);
		return true;
	}

	if (!LoadPictureFromFile(texPath, texRes))
	{
		return false;
	}
	return true;
}

int TexLoader::GetGraphHandle(const std::wstring& texPath)const
{
	if (resourceHandleTable_.contains(texPath))
	{
		return resourceHandleTable_.at(texPath);
	}
	return FAILED;
}

int TexLoader::MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height, const std::vector<uint8_t>& colorData)
{
	int handle = GetGraphHandle(resourceName);
	if (handle != FAILED)
	{
		return handle;
	}

	TextureResorce texRes = {};
	CreateScreenBuffer(texRes.resource, width, height, colorData);
	texRes.imageInf.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texRes.imageInf.width = width;
	texRes.imageInf.height = height;

	CreateSRV(texRes);

	texResources_.emplace_back(texRes);
	handle = Int32(texResources_.size() - 1);

	resourceHandleTable_[resourceName] = handle;

	return handle;
}

bool TexLoader::CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const std::vector<uint8_t>& colorData)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;// �e�N�X�`���p
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// �����_���J���[�摜�̍쐬
	resource.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	D3D12_CLEAR_VALUE clearValue = { DXGI_FORMAT_R8G8B8A8_UNORM , { 0.0f, 0.0f, 0.0f, 0.0f } };
	H_ASSERT(dev_.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc,
		resource.state,
		&clearValue,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	// GPU�ɏ�������
	H_ASSERT(resource.buffer->WriteToSubresource(0,
		nullptr,	// �S�̈�R�s�[
		colorData.data(),	// ���f�[�^�̃A�h���X
		width * pixelSize,	// 1���C���̃T�C�Y
		static_cast<UINT>(colorData.size()// 1���̃T�C�Y
			)));

	return true;
}

bool TexLoader::CreateGradTexture(Resource& resource)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;// �e�N�X�`���p
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
		Color():r(0), g(0), b(0), a(0) {};
	};

	unsigned int redWidthUint = static_cast<unsigned int>(resDesc.Width);
	vector<Color> col(Uint64(redWidthUint) * 256);
	auto it = col.begin();
	for (int j = 255; j >= 0; j--)
	{
		uint8_t u8j = static_cast<uint8_t>(j);
		fill(it, it + redWidthUint, Color(u8j, u8j, u8j, 255));
		it += redWidthUint;
	}

	H_ASSERT(dev_.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	// GPU�ɏ�������
	H_ASSERT(resource.buffer->WriteToSubresource(0,
		nullptr,	// �S�̈�R�s�[
		col.data(),	// ���f�[�^�̃A�h���X
		static_cast<UINT>(sizeof(Color)* resDesc.Width),	// 1���C���̃T�C�Y
		static_cast<UINT>(sizeof(Color)* col.size()	// 1���̃T�C�Y
	)));

	return true;
}

bool TexLoader::CretateLoadLambdaTable()
{
	loadTable_[L"bmp"] = loadTable_[L"sph"] = loadTable_[L"spa"]
		= loadTable_[L"png"] = loadTable_[L"jpg"] = loadTable_[L"jpeg"] =
		[](const wstring& path, TexMetadata* meta, ScratchImage&img)->HRESULT
	{
		return LoadFromWICFile(path.c_str(), WIC_FLAGS::WIC_FLAGS_FORCE_RGB, meta, img);
	};
	loadTable_[L"tga"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};
	loadTable_[L"dds"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT
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
		// �O���{������Ă���̏ꍇ
		// ���Ƃ�����UPLOADBuffa�𒆊ԃo�b�t�@�Ƃ��ėp����
		// CopyTecureRegion�œ]������
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	}
	else
	{
		//WriteToSubresouse����
		// �O���{����̌^�̏ꍇ��CUSTOM
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	}

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	resDesc.Width = img->width;	// ���\�[�X�̃T�C�YX
	resDesc.Height = static_cast<UINT>(img->height);	// ���\�[�X�̃T�C�YY
	resDesc.Format = metaData.format;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);
	resDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);
	resDesc.SampleDesc.Quality = 0;
	resDesc.SampleDesc.Count = 1;

	resource.state = isDiscrete ?
		D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	H_ASSERT(dev_.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		resource.state,
		nullptr,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	return true;
}

// �摜�̓ǂݍ���
bool TexLoader::LoadPictureFromFile(const std::wstring& texPath, TextureResorce& texRes)
{
	// �摜�ǂݍ���
	TexMetadata metaData = {};
	ScratchImage scratchImg = {};

	int idx = static_cast<int>(texPath.find_last_of(L'.'));
	auto ext = texPath.substr(static_cast<size_t>(idx) + 1, texPath.length() - idx - 1);
	if (FAILED(loadTable_[ext](texPath, &metaData, scratchImg)))
	{
		//assert(false);
		return false;
	}

	auto img = scratchImg.GetImage(0, 0, 0);	// ���f�[�^�̒��o
	texRes.imageInf = *img;

	// �ǂݍ��񂾏������ƂɃe�N�X�`���̍쐬
	if (!CreateTextureFromImageData(scratchImg, texRes.resource, metaData, true))
	{
		//assert(false);
		return false;
	}

	// �]���̂��߂̒��ԃo�b�t�@�̍쐬
	auto imgRowPitch = AlignmentValue(static_cast<UINT>(img->rowPitch), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(imgRowPitch * img->height);

	ComPtr<ID3D12Resource> uploadbuff = nullptr;
	H_ASSERT(dev_.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// cpu����̏������݉\
		nullptr,
		IID_PPV_ARGS(uploadbuff.ReleaseAndGetAddressOf())));

	uint8_t* mapforImg = nullptr;//image->pixels�Ɠ����^�ɂ���
	uploadbuff->Map(0, nullptr, (void**)&mapforImg);//�}�b�v
	auto address = img->pixels;
	for (unsigned int j = 0; j < img->height; j++)
	{
		copy_n(address, img->rowPitch, mapforImg);
		address += img->rowPitch;
		mapforImg += imgRowPitch;
	}
	//copy_n(img->pixels, img->slicePitch, mapforImg);//�R�s�[
	uploadbuff->Unmap(0, nullptr);//�A���}�b�v


	D3D12_TEXTURE_COPY_LOCATION src = {}, dst = {};

	// �R�s�[��
	src.pResource = uploadbuff.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData.depth);
	src.PlacedFootprint.Footprint.RowPitch = Uint32(imgRowPitch);
	src.PlacedFootprint.Footprint.Format = img->format;


	// �R�s�[��
	dst.pResource = texRes.resource.buffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// �R�s�[����
	auto& cmdList = cmd_.CommandList();
	cmdList.CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// �o���A�̐ݒ�
	texRes.resource.Barrier(cmd_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	cmd_.Execute();

	return true;
}

int TexLoader::LoadGraph(const std::wstring& path)
{
	int handle = GetGraphHandle(path);
	if (handle != FAILED)
	{
		return handle;
	}

	TextureResorce texRes;
	if (!GetTextureResouse(path, texRes))
	{
		return FAILED;
	}

	// Resource������SRV���ɍ쐬
	CreateSRV(texRes);

	texResources_.emplace_back(texRes);
	resourceHandleTable_[path] = Int32(texResources_.size() - 1);

	return resourceHandleTable_[path];
}

void TexLoader::CreateSRV(TextureResorce& texRes)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Format = texRes.resource.buffer->GetDesc().Format;

	auto incSize = dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto cpuHandle = texHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += texResources_.size() * incSize;
	dev_.CreateShaderResourceView(texRes.resource.buffer.Get(), &srvDesc, cpuHandle);

	texRes.cpuHandleForTex = cpuHandle;
	texRes.gpuHandleForTex = texHeap_->GetGPUDescriptorHandleForHeapStart();
	texRes.gpuHandleForTex.ptr += texResources_.size() * incSize;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texRes.resource.buffer->GetDesc().Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	incSize = dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	cpuHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += texResources_.size() * incSize;
	dev_.CreateRenderTargetView(texRes.resource.buffer.Get(), &rtvDesc, cpuHandle);
	texRes.cpuHandleForRtv = cpuHandle;
	texRes.gpuHandleForRtv = rtvHeap_->GetGPUDescriptorHandleForHeapStart();
	texRes.gpuHandleForRtv.ptr += texResources_.size() * incSize;
}

bool TexLoader::CreateSwapChainBuffer(IDXGISwapChain4& swapChain)
{
	DXGI_SWAP_CHAIN_DESC1 swDesc = {};
	swapChain.GetDesc1(&swDesc);

	for (size_t i = 0; i < swDesc.BufferCount; i++)
	{
		TextureResorce texRes;
		H_ASSERT(swapChain.GetBuffer(Uint32(i), IID_PPV_ARGS(texRes.resource.buffer.ReleaseAndGetAddressOf())));
		texRes.resource.state	= D3D12_RESOURCE_STATE_PRESENT;
		texRes.imageInf.width	= swDesc.Width;
		texRes.imageInf.height	= swDesc.Height;
		CreateSRV(texRes);
		texResources_.emplace_back(texRes);
	}

	return true;
}

void TexLoader::ClsDrawScreen()
{
	assert(renderTergetHandle_ >= 0 && renderTergetHandle_ < texResources_.size());

	auto& commandList = cmd_.CommandList();

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	commandList.ClearRenderTargetView(texResources_[renderTergetHandle_].cpuHandleForRtv, clearColor, 0, nullptr);

	// �[�x�o�b�t�@��������
	auto depthH = depthDSVHeap_->GetCPUDescriptorHandleForHeapStart();
	depthH.ptr += dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * Int32(lightDepthFlag);
	commandList.ClearDepthStencilView(
		depthH,
		D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void TexLoader::SetDrawScreen(const int screenH, const bool lightDepth)
{
	if (renderTergetHandle_ >= 0)
	{
		// ���܂ł̃����_�[�^�[�Q�b�g�̃X�e�[�g��PIXEL_SHADER_RESOURCE�ɂ���
		texResources_[renderTergetHandle_].resource.Barrier(cmd_, renderTergetHandle_ >= 2 ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_PRESENT);
		
		if (lightDepthFlag)
		{
			lightDepthResource_.Barrier(cmd_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		else
		{
			depthResouerce_.Barrier(cmd_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}

	if (lightDepth)
	{
		lightDepthResource_.Barrier(cmd_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}
	else
	{
		depthResouerce_.Barrier(cmd_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}
	auto depthH = &depthDSVHeap_->GetCPUDescriptorHandleForHeapStart();
	depthH ->ptr += dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * Int32(lightDepth);
	lightDepthFlag = lightDepth;


	// �Z�b�g���郌���_�[�^�[�Q�b�g�̃X�e�[�g��RENDER_TARGE�ɂ���
	texResources_[screenH].resource.Barrier(cmd_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmd_.CommandList().OMSetRenderTargets(1, &texResources_[screenH].cpuHandleForRtv, false, depthH);
	renderTergetHandle_ = screenH;
}

void TexLoader::ScreenFlip(IDXGISwapChain4& swapChain)
{
	if (renderTergetHandle_ >= 2)
	{
#ifdef _DEBUG
		OutputDebugString("BackScreen�������_�[�^�[�Q�b�g�ɐݒ肳��Ă��܂���\n");
		assert(false);
#endif
		return;
	}

	// �����_�[�^�[�Q�b�g���v���[���g�p�Ƀo���A�𒣂�
	auto& texRes = GetTextureResouse(renderTergetHandle_);
	assert(texRes.resource.state == D3D12_RESOURCE_STATE_RENDER_TARGET);
	texRes.resource.Barrier(cmd_, D3D12_RESOURCE_STATE_PRESENT);

	cmd_.Execute();

	// ����ʂƕ\��ʂ̐؂�ւ�
	swapChain.Present(1, 0);

}

int TexLoader::MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height)
{
	std::vector<uint8_t> colorData(Uint64(width) * height * pixelSize);
	std::fill(colorData.begin(), colorData.end(), 0);
	return MakeScreen(resourceName, width, height, colorData);
}

int TexLoader::GetCurrentRenderTarget() const
{
	return renderTergetHandle_;
}

bool TexLoader::GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const
{
	if (graphH < 0 || graphH >= texResources_.size())
	{
		assert(false);
		return false;
	}

	width	= Uint32(texResources_[graphH].imageInf.width);
	height	= Uint32(texResources_[graphH].imageInf.height);
	return true;
}

bool TexLoader::GetScreenSize(unsigned int& width, unsigned int& height) const
{
	return GetGraphSize(GetCurrentRenderTarget(), width, height);
}

void TexLoader::SetLightDepthTexDescriptorHeap(const UINT rootParamIdx)
{
	auto& commandList = cmd_.CommandList();
	commandList.SetDescriptorHeaps(1, depthSRVHeap_.GetAddressOf());

	auto handle = depthSRVHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * Int32(lightDepthFlag);
	commandList.SetGraphicsRootDescriptorTable(rootParamIdx,handle);
}

void TexLoader::CreateTextureHeap()
{
	const auto imageMax = Application::Instance().GetImageMax();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = imageMax;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	H_ASSERT(dev_.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(texHeap_.ReleaseAndGetAddressOf())));

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	H_ASSERT(dev_.CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(rtvHeap_.ReleaseAndGetAddressOf())));
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
	// �[�x�o�b�t�@�̍쐬
	auto wsize = Application::Instance().GetWindowSize();

	D3D12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, wsize.w, wsize.h);
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// �d�v�炵��
	D3D12_CLEAR_VALUE depthClearValue = {};
	// �[���̍ő�l1�ɂ���
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	depthResouerce_.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	
	if (FAILED(dev_.CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		depthResouerce_.state,
		&depthClearValue,
		IID_PPV_ARGS(depthResouerce_.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// �V���h�E�}�b�v�p
	depthResDesc.Width = SHADOW_RESOLUTION;
	depthResDesc.Height = SHADOW_RESOLUTION;
	lightDepthResource_.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (FAILED(dev_.CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		lightDepthResource_.state,
		&depthClearValue,
		IID_PPV_ARGS(lightDepthResource_.buffer.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// �[�x�X�e���V���r���[�̍쐬
	if (!CreateDSVAndSRV())
	{
		return false;
	}

	return true;
}

bool TexLoader::CreateDSVAndSRV()
{
	// �[�x�o�b�t�@�r���[�̍쐬
	// �f�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 2;		// 0 �͕`��[�x 1 �̓��C�g�[�x
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(dev_.CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(depthDSVHeap_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// �r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	auto DSVhandle = depthDSVHeap_->GetCPUDescriptorHandleForHeapStart();

	// �`��p
	dev_.CreateDepthStencilView(
		depthResouerce_.buffer.Get(),
		&dsvDesc,
		DSVhandle);
	DSVhandle.ptr += dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// �V���h�E�}�b�v�p
	dev_.CreateDepthStencilView(
		lightDepthResource_.buffer.Get(),
		&dsvDesc,
		DSVhandle);

	//  SR�p�f�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2;		// 0 �͕`��[�x 1 �̓��C�g�[�x
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	if (FAILED(dev_.CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(depthSRVHeap_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// SR�p�̃r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	auto SRVhandle = depthSRVHeap_->GetCPUDescriptorHandleForHeapStart();

	// �`��p
	dev_.CreateShaderResourceView(
		depthResouerce_.buffer.Get(),
		&srvDesc,
		SRVhandle);
	SRVhandle.ptr += dev_.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �V���h�E�}�b�v�p
	dev_.CreateShaderResourceView(
		lightDepthResource_.buffer.Get(),
		&srvDesc,
		SRVhandle);

	return true;
}