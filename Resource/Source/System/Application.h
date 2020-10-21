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

	/// <summary>
	/// �E�B���h�E�T�C�Y�̎擾
	/// </summary>
	Size GetWindowSize();

	/// <summary>
	/// 1�t���[���ɏo����ő�摜���̎擾
	/// </summary>
	/// <returns></returns>
	UINT GetImageMax();

	/// <summary>
	/// DirectX12�Ǘ��N���X�̎擾
	/// </summary>
	Dx12Wrapper& GetDx12();

	// �A�v���P�[�V����������������

	/// <summary>
	/// �A�v���P�[�V����������������
	/// </summary>
	bool Initialize();

	/// <summary>
	/// �A�v���P�[�V���������s����
	/// </summary>
	void Run();

	/// <summary>
	/// �I������
	/// </summary>
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

