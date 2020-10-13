#include "Application.h"
#include "Dx12Wrapper.h"
#include "TexLoader.h"
#include "Drawer.h"
#include "FPSManager.h"
#include "Scene/SceneController.h"
#include <d3d12.h>

using namespace std;

namespace
{
	constexpr unsigned int window_width = 1280;
	constexpr unsigned int window_height = 720;

	constexpr unsigned int FPS = 60;
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		// window�̃n���h����Ԃ��������I��
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}


Application::Application():_imageMax(512)
{
	_hwnd = {};
	_wndClass = {};
}


Application::Size Application::GetWindowSize()
{
	return Size(window_width, window_height);
}

UINT Application::GetImageMax()
{
	return _imageMax;
}

Dx12Wrapper& Application::GetDx12()
{
	return *_dx12;
}

bool Application::Initialize()
{
	// windows�̋@�\���g�킹�Ă��炤
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		return false;
	}

	// �n���h���̎擾(�쐬)
	_wndClass.hInstance = GetModuleHandle(0);
	//�����̃T�C�Y��n��
	_wndClass.cbSize = sizeof(WNDCLASSEX);
	// window�Ɋ֐��|�C���^��n���Ă��� �E�B���h�E�́~�{�^���������ꂽ���Ȃǂɒʒm�����
	_wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	// �A�v���P�[�V�����N���X��
	_wndClass.lpszClassName = ("DirectX");
	//�ݒ��n��
	RegisterClassEx(&_wndClass);

	RECT wrc = {};
	wrc.left = 0;
	wrc.top = 0;
	wrc.right = window_width;
	wrc.bottom = window_height;
	// �E�B���h�E���ٍ��݂̃T�C�Y���擾����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�n���h���̎擾
	_hwnd = CreateWindow(_wndClass.lpszClassName,	// �N���X���̐ݒ�
		"1816030_�x����",		// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ��E���̂���E�B���h�E
		CW_USEDEFAULT,			// X���W��os�ɔC����
		CW_USEDEFAULT,			// y���W��os�ɔC����
		wrc.right - wrc.left,	// �E�B���h�E����
		wrc.bottom - wrc.top,	// �E�B���h�E�c��
		nullptr,				// �e�E�B���h�E�n���h��
		nullptr,				// ���j���[�E�B���h�E�n���h��
		_wndClass.hInstance,	// �Ăяo���A�v���P�[�V�����n���h��
		nullptr					// �ǉ��p�����[�^�[
	);

	// �E�B���h�E�n���h�����擾�ł��Ȃ����false��Ԃ�
	if (_hwnd == 0)return false;

	_dx12 = make_unique<Dx12Wrapper>(_hwnd);
	_dx12->Init();

	_fpsManager.reset(new FPSManager(FPS));

	_sceneController = make_unique<SceneController>();

	return true;
}

void Application::Run()
{// �E�B���h�E�̕\��
	ShowWindow(_hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
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

		_sceneController->SceneUpdate();

		_fpsManager->Wait();
	}
}

void Application::Terminate()
{
}

Application::~Application()
{
}
