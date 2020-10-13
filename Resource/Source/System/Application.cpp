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
		// windowのハンドルを返し処理を終了
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
	// windowsの機能を使わせてもらう
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		return false;
	}

	// ハンドルの取得(作成)
	_wndClass.hInstance = GetModuleHandle(0);
	//自分のサイズを渡す
	_wndClass.cbSize = sizeof(WNDCLASSEX);
	// windowに関数ポインタを渡している ウィンドウの×ボタンが押された時などに通知される
	_wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	// アプリケーションクラス名
	_wndClass.lpszClassName = ("DirectX");
	//設定を渡す
	RegisterClassEx(&_wndClass);

	RECT wrc = {};
	wrc.left = 0;
	wrc.top = 0;
	wrc.right = window_width;
	wrc.bottom = window_height;
	// ウィンドウﾀｲﾄﾙ込みのサイズを取得する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウハンドルの取得
	_hwnd = CreateWindow(_wndClass.lpszClassName,	// クラス名の設定
		"1816030_堀島渚",		// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// タイトルバーと境界線のあるウィンドウ
		CW_USEDEFAULT,			// X座標をosに任せる
		CW_USEDEFAULT,			// y座標をosに任せる
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューウィンドウハンドル
		_wndClass.hInstance,	// 呼び出しアプリケーションハンドル
		nullptr					// 追加パラメーター
	);

	// ウィンドウハンドルが取得できなければfalseを返す
	if (_hwnd == 0)return false;

	_dx12 = make_unique<Dx12Wrapper>(_hwnd);
	_dx12->Init();

	_fpsManager.reset(new FPSManager(FPS));

	_sceneController = make_unique<SceneController>();

	return true;
}

void Application::Run()
{// ウィンドウの表示
	ShowWindow(_hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
	{
		// OS からのメッセージを msg に格納
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// 翻訳
			DispatchMessage(&msg);	// 処理されなかったメッセージをOSに投げ返す
		}

		// アプリケーションが終了しているならWM_QUITになる
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
