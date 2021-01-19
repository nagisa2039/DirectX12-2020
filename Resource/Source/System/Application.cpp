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
		// windowのハンドルを返し処理を終了
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
	// windowsの機能を使わせてもらう
	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		return false;
	}

	// ハンドルの取得(作成)
	wndClass_.hInstance = GetModuleHandle(0);
	//自分のサイズを渡す
	wndClass_.cbSize = sizeof(WNDCLASSEX);
	// windowに関数ポインタを渡している ウィンドウの×ボタンが押された時などに通知される
	wndClass_.lpfnWndProc = (WNDPROC)WindowProcedure;
	// アプリケーションクラス名
	wndClass_.lpszClassName = ("DirectX");
	//設定を渡す
	RegisterClassEx(&wndClass_);

	RECT wrc = {};
	wrc.left = 0;
	wrc.top = 0;
	wrc.right = window_width;
	wrc.bottom = window_height;
	// ウィンドウﾀｲﾄﾙ込みのサイズを取得する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウハンドルの取得
	hwnd_ = CreateWindow(wndClass_.lpszClassName,	// クラス名の設定
		"1816030_堀島渚",		// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// タイトルバーと境界線のあるウィンドウ
		CW_USEDEFAULT,			// X座標をosに任せる
		CW_USEDEFAULT,			// y座標をosに任せる
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューウィンドウハンドル
		wndClass_.hInstance,	// 呼び出しアプリケーションハンドル
		nullptr					// 追加パラメーター
	);

	// ウィンドウハンドルが取得できなければfalseを返す
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
	// ウィンドウの表示
	ShowWindow(hwnd_, SW_SHOW);

	MSG msg = {};
	while (true && !input_->GetButton(DIK_ESCAPE))
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
	// 積まれている命令を実行してからにしておく
	dx12_->GetCommand().Execute();
	CoUninitialize();
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);
}
