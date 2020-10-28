#pragma once
#include <unordered_map>
#include <vector>
#include <xaudio2.h>
#include "Utility/ComPtr.h"

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	int LoadWave(const std::wstring& filePath, bool loop = false);

	bool PlayWave(const int handle)const;
	bool CheckHandleInRange(const int handle) const;
	bool StopSound(const int handle)const;

private:
	ComPtr<IXAudio2> xaudio2_;
	IXAudio2MasteringVoice* masteringVoice_ = nullptr;

	struct SoundData
	{
		IXAudio2SourceVoice* sourceVoice = nullptr;
		XAUDIO2_BUFFER buffer = {};
		std::vector<BYTE> byteData;
	};

	std::unordered_map<std::wstring, int> resourceHandleTable_;
	std::vector<SoundData> soundDatas_;
};

