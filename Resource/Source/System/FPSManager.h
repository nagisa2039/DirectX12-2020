#pragma once
class Application;

class FPSManager
{
	friend Application;

public:
	~FPSManager() = default;


	/// <summary>
	/// �Œ肵���t���[�������擾
	/// </summary>
	int FixedFPS();

	/// <summary>
	/// ���݂̃t���[�����̎擾
	/// </summary>
	float GetFPS();

	/// <summary>
	/// 1�t���[�����̌o�ߎ��Ԃ̎擾
	/// </summary>
	float GetDeltaTime();

private:
	/// <summary>
	/// FPS�Ǘ��N���X
	/// </summary>
	/// <param name="fps">�Œ肵�����t���[����
	/// ���̒l�Ȃ疳����</param>
	FPSManager(const int fps);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const int fixedFPS_;

	/// <summary>
	/// �J�n�~���b
	/// </summary>
	unsigned int startTime_;

	/// <summary>
	/// �O�t���[���̊J�n�~���b
	/// </summary>
	unsigned int prevFrameStartTime_;

	float deltaTime_;

	/// <summary>
	/// �w�肵���t���[�����ɂȂ�܂ő҂�
	/// </summary>
	void Wait();

};

