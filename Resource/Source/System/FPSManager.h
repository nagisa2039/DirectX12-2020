#pragma once
class Application;

class FPSManager
{
	friend Application;

public:
	~FPSManager() = default;

	/// <summary>
	/// �w�肵���t���[�����ɂȂ�܂ő҂�
	/// </summary>
	void Wait();

	/// <summary>
	/// �Œ肵���t���[�������擾
	/// </summary>
	int FixedFPS();

	float GetFPS();

private:
	/// <summary>
	/// FPS�Ǘ��N���X
	/// </summary>
	/// <param name="fps">�Œ肵�����t���[����
	/// ���̒l�Ȃ疳����</param>
	FPSManager(const int fps);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const int _fixedFPS;

	/// <summary>
	/// �J�n�~���b
	/// </summary>
	unsigned int _startTime;

	/// <summary>
	/// �O�t���[���̊J�n�~���b
	/// </summary>
	unsigned int _prevFrameStartTime;

	float _deltaTime;

};

