#pragma once
class Application;

/// <summary>
/// FPS�Ǘ��N���X
/// </summary>
class FPSManager
{
	friend Application;

public:
	~FPSManager() = default;

	/// <summary>
	/// �Œ肵���t���[�������擾
	/// </summary>
	const unsigned int FixedFPS()const;

	/// <summary>
	/// ���݂̃t���[�����̎擾
	/// </summary>
	const float GetFPS()const;

	/// <summary>
	/// 1�t���[�����̌o�ߎ��Ԃ̎擾
	/// </summary>
	const float GetDeltaTime()const;

private:
	/// <param name="fps">�Œ肵�����t���[����
	/// 0�Ȃ疳����</param>
	FPSManager(const unsigned int fps = 60);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const unsigned int fixedFPS_;

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

