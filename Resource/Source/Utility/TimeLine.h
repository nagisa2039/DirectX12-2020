#pragma once
#include <vector>
#include <algorithm>
#include "System/Application.h"
#include "System/FPSManager.h"

template<typename T>

/// <summary>
/// タイムライントラッククラス
/// 値の補完をしてくれる
/// </summary>
/// <typeparam name="T">扱う型</typeparam>
class TimeLine
{
private:
	float time_;
	using key = std::pair<float, T>;
	std::vector<key> keys_;
	bool loop_;
	bool end_;
	bool reverse_;

public:
	/// <summary>
	/// タイムライントラッククラス
	/// 値をフレームごとに追加し線形保管させた値を取得できる
	/// </summary>
	/// <param name="loop">ループさせるか</param>
	inline TimeLine(const bool loop = false)
	{
		time_ = 0.0f;
		keys_.clear();
		loop_ = loop;
		end_ = false;
		reverse_ = false;
	}

	inline ~TimeLine()
	{
	}

	/// <summary>
	/// 更新
	/// </summary>
	inline void Update()
	{
		time_ += Application::Instance().GetFPSManager().GetDeltaTime();
		end_ = time_ >= keys_.rbegin()->first;
	}

	/// <summary>
	/// リセット
	/// </summary>
	inline void Reset()
	{
		time_ = 0.0f;
		end_ = false;
	}

	/// <summary>
	/// キーの追加
	/// </summary>
	/// <param name="frame">秒数</param>
	/// <param name="value">値</param>
	inline void AddKey(const float time, const T value)
	{
		keys_.emplace_back(std::make_pair(time, value));

		auto fuc = [](const key& l, const key& r)
		{
			return l.first < r.first;
		};

		std::sort(keys_.begin(), keys_.end(), fuc);
	}

	/// <summary>
	/// キーの削除
	/// </summary>
	inline void ClearKey()
	{
		keys_.clear();
	}

	/// <summary>
	/// 現在のフレーム数で値を取得
	/// </summary>
	/// <returns></returns>
	inline T GetValue()
	{
		if (keys_.size() <= 0)
		{
#ifdef _DEBUG
			OutputDebugString("TimeLine Keys is Zero\n");
#endif
			return T();
		}

		auto lastTime = keys_.rbegin()->first;
		auto calTime = loop_ ? fmod(time_, lastTime) : time_;
		if (calTime < 0.0f)
		{
			calTime = 0.0f;
		}
		if (calTime > lastTime)
		{
			calTime = lastTime;
		}
		calTime = reverse_ ? lastTime - calTime : calTime;

		auto it = find_if(keys_.rbegin(), keys_.rend(), [calTime](const key key)
		{
			return calTime >= key.first;
		});

		// 指定フレームのアニメーションが無いのでとばす
		if (it == keys_.rend())
		{
			return T();
		}

		auto current = it;
		auto next = it.base();

		if (keys_.end() == next)
		{
			return current->second;
		}

		auto parsent = (calTime - current->first) / (float)(next->first - current->first);
		return current->second + (next->second - current->second) * parsent;
	}

	/// <summary>
	/// 終了しているかを取得
	/// </summary>
	inline bool GetEnd()const
	{
		return end_;
	}

	/// <summary>
	/// ループしているかを取得
	/// </summary>
	/// <returns></returns>
	inline bool GetLoop()const
	{
		return loop_;
	}

	/// <summary>
	/// 逆再生するかを設定
	/// </summary>
	/// <param name="value">逆再生か</param>
	inline void SetReverse(const bool value)
	{
		reverse_ = value;
	}

	/// <summary>
	/// 逆再生になっているかを取得
	/// </summary>
	inline bool GetReverse()const
	{
		return reverse_;
	}

	/// <summary>
	/// 現在の秒数取得
	/// </summary>
	inline float GetTime()const
	{
		return time_;
	}

	/// <summary>
	/// 終了させる
	/// </summary>
	inline void End()
	{
		time_ = keys_.rbegin()->first;
		end_ = true;
	}
};
