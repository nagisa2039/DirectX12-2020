#pragma once
class Application;

/// <summary>
/// FPS管理クラス
/// </summary>
class FPSManager
{
	friend Application;

public:
	~FPSManager() = default;

	/// <summary>
	/// 固定したフレーム数を取得
	/// </summary>
	const unsigned int FixedFPS()const;

	/// <summary>
	/// 現在のフレーム数の取得
	/// </summary>
	const float GetFPS()const;

	/// <summary>
	/// 1フレーム内の経過時間の取得
	/// </summary>
	const float GetDeltaTime()const;

private:
	/// <param name="fps">固定したいフレーム数
	/// 0なら無制限</param>
	FPSManager(const unsigned int fps = 60);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const unsigned int fixedFPS_;

	/// <summary>
	/// 開始ミリ秒
	/// </summary>
	unsigned int startTime_;

	/// <summary>
	/// 前フレームの開始ミリ秒
	/// </summary>
	unsigned int prevFrameStartTime_;

	float deltaTime_;

	/// <summary>
	/// 指定したフレーム数になるまで待つ
	/// </summary>
	void Wait();

};

