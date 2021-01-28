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
/// �e�N�X�`���Ǘ��N���X
/// </summary>
class TexLoader
{
public:
	/// <param name="dev">�f�o�C�X</param>
	/// <param name="cmd">�R�}���h�Ǘ��N���X</param>
	/// <param name="swapChain">�X���b�v�`�F�C��</param>
	TexLoader(ID3D12Device& dev, Command& cmd, IDXGISwapChain4& swapChain);
	~TexLoader();

	// �[�x
	enum class DepthType
	{
		// �J����
		camera,
		// ���C�g
		light,
		// �g�p���Ȃ�
		max
	};

	/// <summary>
	/// �e�N�X�`���Ǘ��ŃX�N���v�^�q�[�v�̎擾
	/// </summary>
	const ComPtr<ID3D12DescriptorHeap>& GetTextureHeap()const;

	/// <summary>
	/// �_�~�[�e�N�X�`���n���h�������̎擾
	/// </summary>
	const DummyTextures& GetDummyTextureHandles()const;

	/// <summary>
	/// �e�N�X�`�����\�[�X�̎擾
	/// </summary>
	/// <param name="handle">�摜�n���h��</param>
	const TextureResorce& GetTextureResouse(const int handle)const;

	/// <summary>
	/// �e�N�X�`�����\�[�X�̎擾
	/// </summary>
	/// <param name="texPath">�e�N�X�`���p�X</param>
	/// <param name="texRes">�e�N�X�`�����\�[�X�̊i�[��</param>
	/// <returns>����</returns>
	bool GetTextureResouse(const std::wstring& texPath, TextureResorce& texRes);

	/// <summary>
	/// �摜�n���h���̎擾
	/// ���Ǎ��Ȃ�ǂݍ���
	/// </summary>
	/// <param name="path">�e�N�X�`���p�X</param>
	/// <returns>�摜�n���h��</returns>
	int GetGraphHandle(const std::wstring& path);

	/// <summary>
	/// ���ݐݒ肳��Ă���`�����N���A(���Z�b�g)����
	/// </summary>
	void ClsDrawScreen();

	/// <summary>
	/// �`���̐ݒ�
	/// </summary>
	/// <param name="screenH">�`���摜�n���h��</param>
	/// <param name="depth">�g�p����[�x</param>
	void SetDrawScreen(const int screenH, const DepthType depth = DepthType::max);
	
	/// <summary>
	/// �`���̐ݒ�
	/// �������ݒ�
	/// </summary>
	/// <param name="screenHList">�`���摜�n���h�����X�g</param>
	/// <param name="depth">�g�p����[�x</param>
	void SetDrawScreen(const std::list<int>& screenHList, const DepthType depth = DepthType::max);
	
	/// <summary>
	/// SwapChain�̃v���b�v
	/// </summary>
	/// <param name="swapChain"></param>
	void ScreenFlip(IDXGISwapChain4& swapChain);

	/// <summary>
	/// �`���摜�̍쐬
	/// </summary>
	/// <param name="resourceName">�g�p��</param>
	/// <param name="width">��</param>
	/// <param name="height">����</param>
	/// <returns>�摜�n���h��</returns>
	int MakeScreen(const std::wstring& resourceName, const UINT width, const UINT height);

	/// <summary>
	/// ���ݐݒ肳��Ă��郌���_�[�^�[�Q�b�g�̈�ڂ̃n���h����Ԃ�
	/// </summary>
	int GetCurrentRenderTarget()const;

	/// <summary>
	/// ���ݐݒ肳��Ă��郌���_�[�^�[�Q�b�g�̔z��擾
	/// </summary>
	std::list<int> GetCurrentRendetTargeAll()const;

	/// <summary>
	/// �摜�T�C�Y�̎擾
	/// </summary>
	/// <param name="graphH">�摜�n���h��</param>
	/// <param name="width">���i�[�ϐ�</param>
	/// <param name="height">�����i�[�ϐ�</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool GetGraphSize(const int graphH, unsigned int& width, unsigned int& height)const;
	
	/// <summary>
	/// ���݂̃����_�[�^�[�Q�b�g�T�C�Y�̎擾
	/// </summary>
	/// <param name="width">���i�[�ϐ�</param>
	/// <param name="height">�����i�[�ϐ�</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool GetScreenSize(unsigned int& width, unsigned int& height)const;

	/// <summary>
	/// �[�x�ŃX�N���v�^�q�[�v�̐ݒ�
	/// </summary>
	/// <param name="rootParamIdx">�p�����[�^�C���f�b�N�X</param>
	/// <param name="depth">�[�x�^�C�v</param>
	void SetDepthTexDescriptorHeap(const UINT rootParamIdx, const DepthType depth);
	
	/// <summary>
	/// �e�N�X�`���z��̃f�X�N���v�^�q�[�v��ݒ�
	/// </summary>
	/// <param name="rootParamIdx">�p�����[�^�C���f�b�N�X</param>
	void SetTextureDescriptorHeap(const UINT rootParamIdx);

private:
	ID3D12Device& dev_;
	Command& cmd_;

	TexLoader(const TexLoader&) = delete;
	TexLoader& operator=(const TexLoader&) = delete;

	bool Init();

	bool CreateDummyTextures();

	// �摜���[�h�plambda�i�[�}�b�v
	using LoadLambda_t = std::function<HRESULT(const std::wstring&, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	std::unordered_map<std::wstring, LoadLambda_t> loadTable_;

	// �e�N�X�`���o�b�t�@
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

	// �X�N���[���p�o�b�t�@�̐����@�쐬��o�b�t�@	��ذ݂̕��ƍ���		�������F
	bool CreateScreenBuffer(Resource& resource, const UINT width, const UINT height, const std::vector<uint8_t>& colorData);
	
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

	void CreateView(TextureResorce& texRes);
	bool CreateSwapChainBuffer(IDXGISwapChain4& swapChain);

	/// <summary>
	/// �w�肵���摜�n���h�������邩�m�F����
	/// </summary>
	/// <param name="texPath">�ǂݍ��ݎ��̃p�X</param>
	/// <returns>�摜�n���h��</returns>
	int FindGraphHandle(const std::wstring& texPath)const;

	// �e�N�X�`�����\�[�X�����݂��邩�m�F
	bool ExistTextureResource(const int handle)const;
};