#pragma once
#include <unordered_map>
#include <memory>
#include <xaudio2.h>
#include "Utility/ComPtr.h"

/// <summary>
/// ���Ǘ��N���X
/// </summary>
class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	/// <summary>
	/// Wave�t�@�C���̓ǂݍ���
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X</param>
	/// <param name="loop">���[�v�ݒ�</param>
	/// <returns>���n���h��</returns>
	int LoadWave(const std::wstring& filePath, bool loop = false);

	/// <summary>
	/// �Đ�
	/// </summary>
	/// <param name="handle">���n���h��</param>
	/// <returns>����</returns>
	bool PlayWave(const int handle);

	/// <summary>
	/// ��~
	/// </summary>
	/// <param name="handle">���n���h��</param>
	/// <returns>����</returns>
	bool StopSound(const int handle);

	/// <summary>
	/// �����Đ�����Ă��邩���m�F
	/// </summary>
	/// <param name="handle">���n���h��</param>
	bool CheckPlaySound(const int handle);

private:
	ComPtr<IXAudio2> xaudio2_;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;

	struct SoundData
	{
		IXAudio2SourceVoice* sourceVoice = nullptr;
		XAUDIO2_BUFFER buffer = {};
		std::unique_ptr<uint8_t[]> data;
		bool isPlaying = false;
	};

	std::unordered_map<std::wstring, int> resourceHandleTable_;
	std::vector<SoundData> soundDatas_;

	// �n���h�����͈͓����m�F
	bool CheckHandleInRange(const int handle) const;
};

