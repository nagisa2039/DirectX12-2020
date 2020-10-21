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

	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	const DummyTextures& GetDummyTextures()const;
	TextureResorce& GetTextureResouse(const int handle);
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);

	int GetGraphHandle(const std::wstring& texPath)const;

	int LoadGraph(const std::wstring& path);

	void ClsDrawScreen();
	void SetDrawScreen(const int screenH);
	void ScreenFlip(IDXGISwapChain4& swapChain);

	int MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height);

	int GetCurrentRenderTarget()const;
	bool GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const;
	bool GetScreenSize(unsigned int& width, unsigned int& height)const;

private:
	ID3D12Device& dev_;
	Command& cmd_;

	TexLoader(const TexLoader&) = delete;
	TexLoader& operator=(const TexLoader&) = delete;

	bool Init();

	// �摜���[�h�plambda�i�[�}�b�v
	using LoadLambda_t = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::unordered_map<std::wstring, LoadLambda_t> loadTable_;

	// �e�N�X�`���o�b�t�@
	std::unordered_map<std::wstring, int> resouseHandleTable_;
	DummyTextures dummyTextures_;

	std::vector<TextureResorce> texResources_;
	ComPtr<ID3D12DescriptorHeap> texHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;

	int renderTergetHandle_;

	// �`��p�[�x�o�b�t�@
	Resource depthResouerce_;
	// �V���h�E�}�b�v�p�[�x�o�b�t�@
	Resource lightDepthResource_;

	ComPtr<ID3D12DescriptorHeap> depthDSVHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> depthSRVHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> lightDSVHeap_ = nullptr;
	ComPtr<ID3D12DescriptorHeap> lightSRVHeap_ = nullptr;

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

	void CreateSRV(TextureResorce& texRes);
	bool CreateSwapChainBuffer(IDXGISwapChain4& swapChain);
};