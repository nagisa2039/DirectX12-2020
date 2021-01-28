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
	int FixedFPS();

	/// <summary>
	/// 現在のフレーム数の取得
	/// </summary>
	float GetFPS();

	/// <summary>
	/// 1フレーム内の経過時間の取得
	/// </summary>
	float GetDeltaTime();

private:
	/// <param name="fps">固定したいフレーム数
	/// 負の値なら無制限</param>
	FPSManager(const int fps);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const int fixedFPS_;

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

