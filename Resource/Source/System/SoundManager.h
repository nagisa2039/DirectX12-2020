#pragma once
#include <unordered_map>
#include <memory>
#include <xaudio2.h>
#include "Utility/ComPtr.h"

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	int LoadWave(const std::wstring& filePath, bool loop = false);

	bool PlayWave(const int handle);
	bool CheckHandleInRange(const int handle) const;
	bool StopSound(const int handle);

	/// <summary>
	/// 音が再生されているかを確認
	/// </summary>
	/// <param name="handle">音ハンドル</param>
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
};

