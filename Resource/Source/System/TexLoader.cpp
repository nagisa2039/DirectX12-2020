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
	// ���e�N�X�`���̍쐬
	if (!CreateScreenBuffer(_dummyTextures.whiteTex, 4, 4 ,255))
	{
		assert(false);
		return false;
	}
	// ���e�N�X�`���̍쐬
	if (!CreateScreenBuffer(_dummyTextures.blackTex, 4, 4, 255))
	{
		assert(false);
		return false;
	}
	// �O���f�[�V�����e�N�X�`���̍쐬
	if (!CreateGradTexture(_dummyTextures.gradTex))
	{
		assert(false);
		return false;
	}

	// �摜�Ǎ��֐��e�[�u���̍쐬
	if (!CretateLoadLambdaTable())
	{
		assert(false);
		return false;
	}

	// �e�N�X�`���p�f�X�N���v�^�q�[�v�̍쐬
	CreateTextureHeap();

	// �[�x�o�b�t�@�̍쐬
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
	// ���\�[�X�e�[���ɂ������炻���Ԃ�
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
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;// �e�N�X�`���p
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// �����_���J���[�摜�̍쐬
	const UINT pixelSize = 4;
	vector<uint8_t> col(width * height * pixelSize);
	fill(col.begin(), col.end(), color);
	resource.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	D3D12_CLEAR_VALUE clearValue = { DXGI_FORMAT_R8G8B8A8_UNORM , { 0.0f, 0.0f, 0.0f, 1.0f } };
	H_ASSERT(_dev.CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc,
		resource.state,
		&clearValue,
		IID_PPV_ARGS(resource.buffer.ReleaseAndGetAddressOf())));

	// GPU�ɏ�������
	H_ASSERT(resource.buffer->WriteToSubresource(0,
		nullptr,	// �S�̈�R�s�[
		col.data(),	// ���f�[�^�̃A�h���X
		width * pixelSize,	// 1���C���̃T�C�Y
		static_cast<UINT>(col.size()// 1���̃T�C�Y
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

	H_ASSERT(_dev.CreateCommittedResource(
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
	auto ext = texPath.substr(idx + 1, texPath.length() - idx - 1);
	if (FAILED(_loadLambdaTable[ext](texPath, &metaData, scratchImg)))
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
	H_ASSERT(_dev.CreateCommittedResource(
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
	src.PlacedFootprint.Footprint.RowPitch = imgRowPitch;
	src.PlacedFootprint.Footprint.Format = img->format;


	// �R�s�[��
	dst.pResource = texRes.resource.buffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// �R�s�[����
	auto& cmdList = _cmd.CommandList();
	cmdList.CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// �o���A�̐ݒ�
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

	// Resource������SRV���ɍ쐬
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

	// �[�x�o�b�t�@��������
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
			// ���܂ł̃����_�[�^�[�Q�b�g�̃X�e�[�g��PIXEL_SHADER_RESOURCE�ɂ���
			_texResources[_renderTergetHandle].resource.Barrier(_cmd, _renderTergetHandle >= 2 ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_PRESENT);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* depthH = &_depthDSVHeap->GetCPUDescriptorHandleForHeapStart();

		// �Z�b�g���郌���_�[�^�[�Q�b�g�̃X�e�[�g��RENDER_TARGE�ɂ���
		_texResources[screenH].resource.Barrier(_cmd, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmd.CommandList().OMSetRenderTargets(1, &_texResources[screenH].cpuHandleForRtv, false, depthH);
		_renderTergetHandle = screenH;
	}
}

void TexLoader::ScreenFlip(IDXGISwapChain4& swapChain)
{
	// swapChain�̃_�u���o�b�t�@�ȊO���w�肳��Ă������O
	assert(_renderTergetHandle < 2);

	// �����_�[�^�[�Q�b�g���v���[���g�p�Ƀo���A�𒣂�
	auto& texRes = GetTextureResouse(_renderTergetHandle);
	assert(texRes.resource.state == D3D12_RESOURCE_STATE_RENDER_TARGET);
	texRes.resource.Barrier(_cmd, D3D12_RESOURCE_STATE_PRESENT);

	_cmd.Execute();

	// ����ʂƕ\��ʂ̐؂�ւ�
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

	// �V���h�E�}�b�v�p
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
	dsvHeapDesc.NumDescriptors = 1;		// 0 �͕`��[�x 1 �̓��C�g�[�x
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(_dev.CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(_depthDSVHeap.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// �r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	auto DSVhandle = _depthDSVHeap->GetCPUDescriptorHandleForHeapStart();

	// �`��p
	_dev.CreateDepthStencilView(
		_depthBuffer.buffer.Get(),
		&dsvDesc,
		DSVhandle);
	DSVhandle.ptr += _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//// �V���h�E�}�b�v�p
	//_dev.CreateDepthStencilView(
	//	_lightDepthBuffer.buffer.Get(),
	//	&dsvDesc,
	//	DSVhandle);

	//  SR�p�f�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = 2;		// 0 �͕`��[�x 1 �̓��C�g�[�x
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	if (FAILED(_dev.CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(_depthSRVHeap.ReleaseAndGetAddressOf()))))
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
	auto SRVhandle = _depthSRVHeap->GetCPUDescriptorHandleForHeapStart();

	// �`��p
	_dev.CreateShaderResourceView(
		_depthBuffer.buffer.Get(),
		&srvDesc,
		SRVhandle);
	SRVhandle.ptr += _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//// �V���h�E�}�b�v�p
	//_dev.CreateShaderResourceView(
	//	_lightDepthBuffer.buffer.Get(),
	//	&srvDesc,
	//	SRVhandle);

	return true;
}