#pragma once
#include <windows.h>
#include <memory>

class Dx12Wrapper;
class FPSManager;
class SceneController;

class Application
{
public:
	struct Size
	{
		unsigned int w, h;
		Size():w(0),h(0) {};
		Size(unsigned int w, unsigned int h) :w(w), h(h) {};
	};

	static Application& Instance()
	{
		static Application instance;
		return instance;
	}

	Size GetWindowSize();
	UINT GetImageMax();

	/// <summary>
	/// DirectX12�Ǘ��N���X�̎擾
	/// </summary>
	Dx12Wrapper& GetDx12();

	// �A�v���P�[�V����������������
	bool Initialize();

	// �A�v���P�[�V���������s����
	void Run();

	// �I������
	void Terminate();

	~Application();

private:
	HWND hwnd_;
	WNDCLASSEX wndClass_;

	std::unique_ptr<Dx12Wrapper> dx12_;
	std::unique_ptr<FPSManager> fpsManager_;
	std::unique_ptr<SceneController> sceneController_;

	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};

