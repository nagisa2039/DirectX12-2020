#include "SoundManager.h"
#include <vector>
#include "Utility/dx12Tool.h"
#include "Utility/Tool.h"
#include <Mmsystem.h>

#pragma comment (lib, "winmm.lib")

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

	HMMIO mmio = NULL;
	MMIOINFO info = {};
	mmio = mmioOpen(StringFromWString(filePath).data(), &info, MMIO_READ);

	if (!mmio)
	{
		assert(false);
		return FAILED;
	}

	MMRESULT mret;
	MMCKINFO riff_chunk;
	riff_chunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mret = mmioDescend(mmio, &riff_chunk, NULL, MMIO_FINDRIFF);
	if (mret != MMSYSERR_NOERROR)
	{
		assert(false);
		return FAILED;
	}

	MMCKINFO chunk;
	chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mret = mmioDescend(mmio, &chunk, &riff_chunk, MMIO_FINDCHUNK);
	if (mret != MMSYSERR_NOERROR)
	{
		assert(false);
		return FAILED;
	}

	WAVEFORMATEX format = {};
	DWORD size = mmioRead(mmio, (HPSTR)&format, chunk.cksize);
	if (size != chunk.cksize)
	{
		assert(false);
		return FAILED;
	}

	chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mret = mmioDescend(mmio, &chunk, &riff_chunk, MMIO_FINDCHUNK);
	if (mret != MMSYSERR_NOERROR)
	{
		assert(false);
		return FAILED;
	}

	IXAudio2SourceVoice* pSourceVoice = nullptr; 
	auto result = xaudio2_->CreateSourceVoice(&pSourceVoice, &format);
	H_ASSERT(result);

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	std::vector<BYTE> byteData(format.nAvgBytesPerSec);

	mmioRead(mmio, (HPSTR)(byteData.data()), byteData.size());
	buf.AudioBytes = byteData.size();
	buf.pAudioData = byteData.data();
	buf.Flags = XAUDIO2_END_OF_STREAM;

	soundDatas_.emplace_back(SoundData{ pSourceVoice, buf, byteData});
	int handle = soundDatas_.size() - 1;
	resourceHandleTable_[filePath] = handle;
	return handle;
}

bool SoundManager::PlayWave(const int handle) const
{
	if (!CheckHandleInRange(handle))return false;

	auto& soundData = soundDatas_[handle];
	auto result = soundData.sourceVoice->SubmitSourceBuffer(&soundData.buffer);
	H_ASSERT(result);
	result = soundData.sourceVoice->Start();
	H_ASSERT(result);

	return true;
}

bool SoundManager::CheckHandleInRange(const int handle) const
{
	return (0 <= handle && handle < soundDatas_.size());
}

bool SoundManager::StopSound(const int handle) const
{
	if (!CheckHandleInRange(handle))return false;

	XAUDIO2_VOICE_STATE state;
	soundDatas_[handle].sourceVoice->GetState(&state);

	soundDatas_[handle].sourceVoice->Stop();
	return true;
}
