#include "SoundManager.h"
#include <vector>
#include "Utility/dx12Tool.h"
#include "Utility/Tool.h"
#include <sstream>
#include "System/WAVFileReader.h"

#pragma comment(lib,"xaudio2.lib")

using namespace std;

SoundManager::SoundManager()
{
	auto result = XAudio2Create(xaudio2_.ReleaseAndGetAddressOf());
	H_ASSERT(result);
	result = xaudio2_->CreateMasteringVoice(&masteringVoice_);
	H_ASSERT(result);

	resourceHandleTable_.clear();
	soundDatas_.clear();
}

SoundManager::~SoundManager()
{
	masteringVoice_->DestroyVoice();

	for (auto& soundData : soundDatas_)
	{
		soundData.sourceVoice->Stop();
		soundData.sourceVoice->DestroyVoice();
	}
}

int SoundManager::LoadWave(const std::wstring& filePath, bool loop)
{
	if (resourceHandleTable_.contains(filePath))
	{
		return resourceHandleTable_[filePath];
	}
	soundDatas_.emplace_back(SoundData{});
	auto& sd = *soundDatas_.rbegin();
	
	DirectX::WAVData waveData;
	if (FAILED(DirectX::LoadWAVAudioFromFileEx(filePath.data(), sd.data, waveData)))
	{
		assert(false);
		return FAILED;
	}

	auto result = xaudio2_->CreateSourceVoice(&sd.sourceVoice, waveData.wfx);
	H_ASSERT(result);

	// 再生する波形データの設定
	sd.buffer = { 0 };
	sd.buffer.pAudioData = waveData.startAudio;
	sd.buffer.Flags = XAUDIO2_END_OF_STREAM;
	sd.buffer.AudioBytes = waveData.audioBytes;


	//result = sd.sourceVoice->SubmitSourceBuffer(&sd.buffer);
	//H_ASSERT(result);
	

	int handle = soundDatas_.size() - 1;
	resourceHandleTable_[filePath] = handle;
	PlayWave(handle);
	return handle;
}

bool SoundManager::PlayWave(const int handle)
{
	if (!CheckHandleInRange(handle))return false;


	auto& soundData = soundDatas_[handle];
	XAUDIO2_VOICE_STATE state;
	soundData.sourceVoice->GetState(&state);
	bool isRunning = state.BuffersQueued > 0;

	soundData.sourceVoice->Stop();
	soundData.sourceVoice->FlushSourceBuffers();
	auto result = soundData.sourceVoice->SubmitSourceBuffer(&soundData.buffer);
	H_ASSERT(result);
	result = soundData.sourceVoice->Start();
	soundData.isPlaying = true;
	H_ASSERT(result);

	return true;
}

bool SoundManager::CheckHandleInRange(const int handle) const
{
	return (0 <= handle && handle < soundDatas_.size());
}

bool SoundManager::StopSound(const int handle)
{
	if (!CheckHandleInRange(handle))return false;

	XAUDIO2_VOICE_STATE state;
	soundDatas_[handle].sourceVoice->GetState(&state);

	soundDatas_[handle].sourceVoice->Stop();
	soundDatas_[handle].isPlaying = false;
	return true;
}
