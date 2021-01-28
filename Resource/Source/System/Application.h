#pragma once
#include <windows.h>
#include <memory>

class Dx12Wrapper;
class FPSManager;
class SceneController;
class Input;
class ShaderLoader;
class Scene;

class Application
{
public:
	// サイズ
	struct Size
	{
		unsigned int w, h;
		Size():w(0),h(0) {};
		Size(unsigned int w, unsigned int h) :w(w), h(h) {};
	};

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}

	/// <summary>
	/// ウィンドウサイズの取得
	/// </summary>
	Size GetWindowSize();

	/// <summary>
	/// 1フレームに出せる最大画像数の取得
	/// </summary>
	/// <returns></returns>
	UINT GetImageMax();

	/// <summary>
	/// DirectX12管理クラスの取得
	/// </summary>
	Dx12Wrapper& GetDx12();

	/// <summary>
	/// 入力管理クラスの取得
	/// </summary>
	/// <returns></returns>
	Input& GetInput();

	/// <summary>
	/// Shader読み込みクラスの取得
	/// </summary>
	ShaderLoader& GetShaderLoader();

	/// <summary>
	/// フレーム管理クラスの取得
	/// </summary>
	FPSManager& GetFPSManager();

	// アプリケーションを初期化する

	/// <summary>
	/// アプリケーションを初期化する
	/// </summary>
	bool Initialize();

	/// <summary>
	/// アプリケーションを実行する
	/// </summary>
	void Run();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Terminate();

	~Application();

private:
	HWND hwnd_;
	WNDCLASSEX wndClass_;

	std::unique_ptr<Dx12Wrapper> dx12_;
	std::unique_ptr<FPSManager> fpsManager_;
	std::unique_ptr<SceneController> sceneController_;
	std::unique_ptr<Input> input_;
	std::unique_ptr<ShaderLoader> shaderLoader_;

	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};

