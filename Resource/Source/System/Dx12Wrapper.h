#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <vector>
#include <memory>
#include "Command.h"
#include <array>
#include <DirectXMath.h>
#include <string>
#include "Utility/TextureStruct.h"
#include "System/FileSystem.h"

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

using Microsoft::WRL::ComPtr;

class Command;
class TexLoader;
class SpriteDrawer;
class SoundManager;
class RendererManager;
class FileSystem;

struct SettingData;

class Dx12Wrapper
{
public:
	/// <summary>
	/// DirectX�Ǘ��N���X
	/// </summary>
	/// <param name="hwnd">�E�B���h�E�n���h��</param>
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Init();

	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	ID3D12Device& GetDevice();

	/// <summary>
	/// �R�}���h�̎擾
	/// </summary>
	Command& GetCommand();

	/// <summary>
	/// �e�N�X�`�����\�[�X�̊Ǘ��N���X�擾
	/// </summary>
	TexLoader& GetTexLoader();

	/// <summary>
	/// �T�E���h�Ǘ�����̎擾
	/// </summary>
	SoundManager& GetSoundManager();

	/// <summary>
	/// 2D�`��Ǘ��N���X�̎擾
	/// </summary>
	SpriteDrawer& GetSpriteDrawer();

	/// <summary>
	/// 3D�`��Ǘ��N���X�̎擾
	/// </summary>
	RendererManager& GetRendererManager();

	/// <summary>
	/// �t�@�C���ǂݍ��݊Ǘ��N���X�̎擾
	/// </summary>
	FileSystem& GetFileSystem();
	
	/// <summary>
	/// ���݂̗���ʃn���h���̎擾
	/// </summary>
	int GetBackScreenHandle();

	/// <summary>
	/// ����ʂƕ\��ʂ̓���ւ�
	/// </summary>
	void ScreenFlip();

	/// <summary>
	/// �G�t�F�N�g�̕`��
	/// </summary>
	void DrawEfk();

	/// <summary>
	/// �J�n����ImGui�̕`��
	/// </summary>
	void BeginDrawImGui();

	/// <summary>
	/// �I����ImGui�̕`��
	/// </summary>
	void EndDrawImGui();

	/// <summary>
	/// �S��ʂ�Viewport��Scissor��ݒ肷��
	/// </summary>
	void SetDefaultViewAndScissor();

	/// <summary>
	/// �w��͈͂ɂ�Viewport��Scissor��ݒ肷��
	/// </summary>
	/// <param name="width">��ʕ�</param>
	/// <param name="height">��ʍ���</param>
	void SetViewAndScissor(const UINT width, const UINT height);

	/// <summary>
	/// �ݒ�f�[�^���Z�b�g����
	/// </summary>
	/// <param name="rootPramIndex">���[�g�V�O�l�`���C���f�b�N�X</param>
	void SetSettingData(const UINT rootPramIndex);

private:
	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<IDXGIFactory6> dxgiFactory_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::shared_ptr<TexLoader> texLoader_;
	std::shared_ptr<SpriteDrawer> spriteDrawer_;
	std::shared_ptr<Command> cmd_;
	std::shared_ptr<SoundManager> soundManager_;
	std::shared_ptr<RendererManager> rendererManager_;
	std::shared_ptr<FileSystem> fileSystem_;

	EffekseerRenderer::Renderer* efkRenderer_ = nullptr;
	Effekseer::Manager* efkManager_ = nullptr;
	EffekseerRenderer::SingleFrameMemoryPool* efkMemoryPool_ = nullptr;
	EffekseerRenderer::CommandList* efkCmdList_ = nullptr;
	Effekseer::Effect* effect_ = nullptr;
	Effekseer::Handle efkHandle_ = {};

	ComPtr<ID3D12DescriptorHeap> imguiHeap_;

	ComPtr<ID3D12DescriptorHeap> settingHeap_;
	ComPtr<ID3D12Resource> settingCB_;
	SettingData* settingData_ = nullptr;

	void CreateSwapChain();

	void InitEfk();

	void InitImGui();

	void CreateSettingData();

	void DrawImGuiForSetting();
};

