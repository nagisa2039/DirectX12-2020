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

using Microsoft::WRL::ComPtr;

class Command;
class TexLoader;
class SpriteDrawer;
class SoundManager;
class Camera;
class RendererManager;

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
	/// �J�����N���X�̎擾
	/// </summary>
	Camera& GetCamera();

	/// <summary>
	/// 3D�`��Ǘ��N���X�̎擾
	/// </summary>
	/// <returns></returns>
	RendererManager& GetRendererManager();
	
	/// <summary>
	/// ���݂̗���ʃn���h���̎擾
	/// </summary>
	int GetBackScreenHandle();

	/// <summary>
	/// ����ʂƕ\��ʂ̓���ւ�
	/// </summary>
	void ScreenFlip();

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

private:
	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<IDXGIFactory6> dxgiFactory_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::shared_ptr<TexLoader> texLoader_;
	std::shared_ptr<SpriteDrawer> spriteDrawer_;
	std::shared_ptr<Command> cmd_;
	std::shared_ptr<SoundManager> soundManager_;
	std::shared_ptr<Camera> camera_;
	std::shared_ptr<RendererManager> rendererManager_;

	void CreateSwapChain();
};

