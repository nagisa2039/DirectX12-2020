#include "SoundManager.h"
#include <vector>
#include "Utility/dx12Tool.h"
#include "Utility/Tool.h"

#pragma comment(lib,"Ole32.lib")

using namespace std;

SoundManager::SoundManager()
{
	H_ASSERT(CoInitializeEx(0, COINITBASE_MULTITHREADED));
	H_ASSERT(XAudio2Create(xaudio2_.ReleaseAndGetAddressOf()));
	IXAudio2MasteringVoice* masteringVoice = nullptr;
	H_ASSERT(xaudio2_->CreateMasteringVoice(&masteringVoice));
	masteringVoice_.reset(masteringVoice);

	resourceHandleTable_.clear();
	soundDatas_.clear();
}

SoundManager::~SoundManager()
{
	CoUninitialize();
	masteringVoice_->DestroyVoice();
	for (auto& soundData : soundDatas_)
	{
		soundData.sourceVoice->Stop();
		soundData.sourceVoice->DestroyVoice();
	}
}

int SoundManager::LoadWave(const std::wstring& filePath)
{
	if (resourceHandleTable_.contains(filePath))
	{
		return resourceHandleTable_[filePath];
	}

	FILE* file = nullptr;
	std::string str = StringFromWString(filePath);
	fopen_s(&file, str.c_str(), "rb");

	if (!file)
	{
		return -1;
	}

# pragma pack(4)
	struct Chunk
	{
		char id[4];
		int size;
	};

	struct RiffHeader
	{
		Chunk chunk;
		char type[4];
	};

	struct FormatChunk
	{
		Chunk chunk;
		WAVEFORMAT fmt;
	};
# pragma pack(8)

	RiffHeader riff = {};
	fread_s(&riff, sizeof(riff), sizeof(RiffHeader), 1, file);

	FormatChunk format = {};
	fread_s(&format, sizeof(format), sizeof(FormatChunk), 1, file);

	Chunk data = {};
	fread_s(&data, sizeof(data), sizeof(Chunk), 1, file);

	std::vector<BYTE> byteData(data.size);
	fread_s(byteData.data(), byteData.size(), data.size, 1, file);

	fclose(file); 

	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	if (FAILED(xaudio2_->CreateSourceVoice(&pSourceVoice, &wfex)))
	{
		return -1;
	}

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = byteData.data();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = data.size;

	soundDatas_.emplace_back(SoundData{ unique_ptr<IXAudio2SourceVoice>(pSourceVoice), buf });
	int handle = soundDatas_.size() - 1;
	resourceHandleTable_[filePath] = handle;
	return handle;
}

bool SoundManager::PlayWave(const int handle) const
{
	if (!CheckHandleInRange(handle))return false;

	auto& soundData = soundDatas_[handle];
	H_ASSERT(soundData.sourceVoice->SubmitSourceBuffer(&soundData.buffer));
	H_ASSERT(soundData.sourceVoice->Start());

	return true;
}

bool SoundManager::CheckHandleInRange(const int handle) const
{
	return (0 <= handle && handle < soundDatas_.size());
}

bool SoundManager::StopSound(const int handle) const
{
	if (!CheckHandleInRange(handle))return false;
	soundDatas_[handle].sourceVoice->Stop();
	return true;
}
