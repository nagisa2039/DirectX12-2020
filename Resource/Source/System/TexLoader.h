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

	// �摜���[�h�plambda�i�[�}�b�v
	using LoadLambda_t = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::unordered_map<std::wstring, LoadLambda_t> _loadLambdaTable;

	// �e�N�X�`���o�b�t�@
	std::unordered_map<std::wstring, int> _resouseHandleTable;
	DummyTextures _dummyTextures;

	std::vector<TextureResorce> _texResources;
	ComPtr<ID3D12DescriptorHeap> _texHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr;

	int _renderTergetHandle;

	// �`��p�[�x�o�b�t�@
	Resource _depthBuffer;
	// �V���h�E�}�b�v�p�[�x�o�b�t�@
	Resource _lightDepthBuffer;

	ComPtr<ID3D12DescriptorHeap> _depthDSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _depthSRVHeap = nullptr;

	// �X�N���[���p�o�b�t�@�̐����@�쐬��o�b�t�@	��ذ݂̕��ƍ���		�������F
	bool CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const int color = 0);
	// �O���f�[�V�����e�N�X�`���̐���	(tonn �������Ƃ��Ɏg���K��)
	bool CreateGradTexture(Resource& resource);

	// �摜���[�h�e�[�u���̍쐬
	bool CretateLoadLambdaTable();

	// �V�F�[�_�[���\�[�X�̍쐬
	bool CreateTextureFromImageData(const DirectX::ScratchImage& scrachImage, Resource& resource, const DirectX::TexMetadata& metaData, bool isDiscread = false);

	// �摜�ǂݍ���
	bool LoadPictureFromFile(const std::wstring& texPath, TextureResorce& texRes);

	void CreateTextureHeap();

	// �[�x�o�b�t�@�̍쐬
	bool CreateDepthBuffer();
	// �[�x�X�e���V���r���[�̍쐬
	bool CreateDSVAndSRV();
};