#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <xaudio2.h>
#include "Utility/ComPtr.h"

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	int LoadWave(const std::wstring& filePath);

	bool PlayWave(const int handle)const;
	bool CheckHandleInRange(const int handle) const;
	bool StopSound(const int handle)const;

private:
	ComPtr<IXAudio2> xaudio2_;
	std::unique_ptr<IXAudio2MasteringVoice> masteringVoice_;

	struct SoundData
	{
		std::unique_ptr<IXAudio2SourceVoice> sourceVoice;
		XAUDIO2_BUFFER buffer;
	};

	std::unordered_map<std::wstring, int> resourceHandleTable_;
	std::vector<SoundData> soundDatas_;
};

