#pragma once
class Application;

class FPSManager
{
	friend Application;

public:
	~FPSManager() = default;

	/// <summary>
	/// 指定したフレーム数になるまで待つ
	/// </summary>
	void Wait();

	/// <summary>
	/// 固定したフレーム数を取得
	/// </summary>
	int FixedFPS();

	float GetFPS();

private:
	/// <summary>
	/// FPS管理クラス
	/// </summary>
	/// <param name="fps">固定したいフレーム数
	/// 負の値なら無制限</param>
	FPSManager(const int fps);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

	const int _fixedFPS;

	/// <summary>
	/// 開始ミリ秒
	/// </summary>
	unsigned int _startTime;

	/// <summary>
	/// 前フレームの開始ミリ秒
	/// </summary>
	unsigned int _prevFrameStartTime;

	float _deltaTime;

};

