#include "Application.h"
#include "Dx12Wrapper.h"
#include "TexLoader.h"
#include "2D/SpriteDrawer.h"
#include "FPSManager.h"
#include "Scene/SceneController.h"
#include "Utility/Input.h"
#include <d3d12.h>
#include "System/ShaderLoader.h"

using namespace std;

namespace
{
	constexpr unsigned int window_width = 1280;
	constexpr unsigned int window_height = 720;

	constexpr unsigned int FPS = 60;
	
	constexpr UINT IMAGE_MAX = 512;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		// window�̃n���h����Ԃ��������I��
		PostQuitMessage(0);
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


Application::Application()
{
	hwnd_ = {};
	wndClass_ = {};
}


Application::Size Application::GetWindowSize()
{
	return Size(window_width, window_height);
}

UINT Application::GetImageMax()
{
	return IMAGE_MAX;
}

Dx12Wrapper& Application::GetDx12()
{
	return *dx12_;
}

Input& Application::GetInput()
{
	return *input_;
}

ShaderLoader& Application::GetShaderLoader()
{
	return *shaderLoader_;
}

FPSManager& Application::GetFPSManager()
{
	return *fpsManager_;
}

bool Application::Initialize()
{
	// windows�̋@�\���g�킹�Ă��炤
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		return false;
	}

	// �n���h���̎擾(�쐬)
	wndClass_.hInstance = GetModuleHandle(0);
	//�����̃T�C�Y��n��
	wndClass_.cbSize = sizeof(WNDCLASSEX);
	// window�Ɋ֐��|�C���^��n���Ă��� �E�B���h�E�́~�{�^���������ꂽ���Ȃǂɒʒm�����
	wndClass_.lpfnWndProc = (WNDPROC)WindowProcedure;
	// �A�v���P�[�V�����N���X��
	wndClass_.lpszClassName = ("DirectX");
	//�ݒ��n��
	RegisterClassEx(&wndClass_);

	RECT wrc = {};
	wrc.left = 0;
	wrc.top = 0;
	wrc.right = window_width;
	wrc.bottom = window_height;
	// �E�B���h�E���ٍ��݂̃T�C�Y���擾����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�n���h���̎擾
	hwnd_ = CreateWindow(wndClass_.lpszClassName,	// �N���X���̐ݒ�
		"1816030_�x����",		// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ��E���̂���E�B���h�E
		CW_USEDEFAULT,			// X���W��os�ɔC����
		CW_USEDEFAULT,			// y���W��os�ɔC����
		wrc.right - wrc.left,	// �E�B���h�E����
		wrc.bottom - wrc.top,	// �E�B���h�E�c��
		nullptr,				// �e�E�B���h�E�n���h��
		nullptr,				// ���j���[�E�B���h�E�n���h��
		wndClass_.hInstance,	// �Ăяo���A�v���P�[�V�����n���h��
		nullptr					// �ǉ��p�����[�^�[
	);

	// �E�B���h�E�n���h�����擾�ł��Ȃ����false��Ԃ�
	if (hwnd_ == 0)return false;

	shaderLoader_ = make_unique<ShaderLoader>();

	dx12_ = make_unique<Dx12Wrapper>(hwnd_);
	dx12_->Init();

	fpsManager_.reset(new FPSManager(FPS));

	sceneController_ = make_unique<SceneController>();
	input_ = make_unique<Input>(hwnd_);

	return true;
}

void Application::Run()
{
	// �E�B���h�E�̕\��
	ShowWindow(hwnd_, SW_SHOW);

	MSG msg = {};
	while (true && !input_->GetButton(DIK_ESCAPE))
	{
		// OS ����̃��b�Z�[�W�� msg �Ɋi�[
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// �|��
			DispatchMessage(&msg);	// ��������Ȃ��������b�Z�[�W��OS�ɓ����Ԃ�
		}

		// �A�v���P�[�V�������I�����Ă���Ȃ�WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}

		input_->Update();
		sceneController_->SceneUpdate();

		fpsManager_->Wait();
	}
}

void Application::Terminate()
{
}

Application::~Application()
{
	// �ς܂�Ă��閽�߂����s���Ă���ɂ��Ă���
	dx12_->GetCommand().Execute();
	CoUninitialize();
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);
}
