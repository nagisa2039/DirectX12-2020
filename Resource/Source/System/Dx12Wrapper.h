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
	/// 2D�`��Ǘ��N���X�̎擾
	/// </summary>
	SpriteDrawer& GetSpriteDrawer();
	
	/// <summary>
	/// ���݂̗���ʃn���h���̎擾
	/// </summary>
	int GetBackScreenHandle();

	/// <summary>
	/// ����ʂƕ\��ʂ̓���ւ�
	/// </summary>
	void ScreenFlip();

	/// <summary>
	/// �J�����̃q�[�v��ݒ�
	/// </summary>
	/// <param name="rootParamIdx">rootSignature�C���f�b�N�X</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// �S��ʂ�Viewport��Scissor��ݒ肷��
	/// </summary>
	void SetDefaultViewAndScissor();

private:
	HWND hwnd_;

	ComPtr<ID3D12Device> dev_ = nullptr;
	ComPtr<IDXGIFactory6> dxgiFactory_ = nullptr;
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::shared_ptr<TexLoader> texLoader_;
	std::shared_ptr<SpriteDrawer> spriteDrawer_;
	std::shared_ptr<Command> cmd_;

	// �J�����s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;
	
	// GPU�ɓn���s����܂Ƃ߂��\����
	struct Scene
	{
		//DirectX::XMMATRIX world;	// �����O���[���h
		DirectX::XMMATRIX view;	//
		DirectX::XMMATRIX proj;	//
		DirectX::XMMATRIX invProj;	// �v���W�F�N�V�����̋t�s��
		DirectX::XMMATRIX lightCamera;	//���C�h���猩���r���[�v���W�F�N�V����
		DirectX::XMMATRIX shadow;
		DirectX::XMFLOAT3 eye;	// ���_
	};

	struct Camera
	{
		DirectX::XMFLOAT3 eye;
		DirectX::XMFLOAT3 target;
		DirectX::XMFLOAT3 up;
		float fov = DirectX::XM_PIDIV4;
	};

	// �萔�o�b�t�@�̃A�h���X���i�[	
	// _cameraCB�̓��e��ύX�������Ƃ��͂�����ʂ��ĕύX���Ă�
	Scene* mappedCam_;

	// ���_(�J�����̈ʒu)
	// �����_(����Ώۂ̈ʒu)
	// ��x�N�g��(��)
	Camera camera_;

	// �J�����̃o�b�t�@�ƃr���[���쐬
	bool CreateCameraConstantBufferAndView();

	void CreateSwapChain();

	void UpdateSceneMatrix();
};

