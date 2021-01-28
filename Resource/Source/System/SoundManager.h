#pragma once
#include <unordered_map>
#include <memory>
#include <xaudio2.h>
#include "Utility/ComPtr.h"

/// <summary>
/// 音管理クラス
/// </summary>
class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	/// <summary>
	/// Waveファイルの読み込み
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="loop">ループ設定</param>
	/// <returns>音ハンドル</returns>
	int LoadWave(const std::wstring& filePath, bool loop = false);

	/// <summary>
	/// 再生
	/// </summary>
	/// <param name="handle">音ハンドル</param>
	/// <returns>成否</returns>
	bool PlayWave(const int handle);

	/// <summary>
	/// 停止
	/// </summary>
	/// <param name="handle">音ハンドル</param>
	/// <returns>成否</returns>
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

	// ハンドルが範囲内か確認
	bool CheckHandleInRange(const int handle) const;
};

