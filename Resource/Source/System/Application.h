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
		Size() {};
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
	HWND _hwnd;
	WNDCLASSEX _wndClass;

	std::unique_ptr<Dx12Wrapper> _dx12;
	std::unique_ptr<FPSManager> _fpsManager;
	std::unique_ptr<SceneController> _sceneController;

	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	const UINT _imageMax;
};

