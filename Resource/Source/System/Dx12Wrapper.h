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

	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	bool Init();

	ID3D12Device& GetDevice();
	Command& GetCommand();

	TexLoader& GetTexLoader();
	SpriteDrawer& GetSpriteDrawer();
	
	int GetBackScreenHandle();
	void ScreenFlip();

	void SetCameraDescriptorHeap(const UINT rootParamIdx);

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

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	// �J�����̃o�b�t�@�ƃr���[���쐬
	bool CreateCameraConstantBufferAndView();

	void CreateSwapChain();

	void UpdateSceneMatrix();
};

