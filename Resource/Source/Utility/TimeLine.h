#pragma once
#include <vector>
#include <algorithm>
#include "System/Application.h"
#include "System/FPSManager.h"

template<typename T>

/// <summary>
/// �^�C�����C���g���b�N�N���X
/// �l�̕⊮�����Ă����
/// </summary>
/// <typeparam name="T">�����^</typeparam>
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
	/// �^�C�����C���g���b�N�N���X
	/// �l���t���[�����Ƃɒǉ������`�ۊǂ������l���擾�ł���
	/// </summary>
	/// <param name="loop">���[�v�����邩</param>
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
	/// �X�V
	/// </summary>
	inline void Update()
	{
		time_ += Application::Instance().GetFPSManager().GetDeltaTime();
		end_ = time_ >= keys_.rbegin()->first;
	}

	/// <summary>
	/// ���Z�b�g
	/// </summary>
	inline void Reset()
	{
		time_ = 0.0f;
		end_ = false;
	}

	/// <summary>
	/// �L�[�̒ǉ�
	/// </summary>
	/// <param name="frame">�b��</param>
	/// <param name="value">�l</param>
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
	/// �L�[�̍폜
	/// </summary>
	inline void ClearKey()
	{
		keys_.clear();
	}

	/// <summary>
	/// ���݂̃t���[�����Œl���擾
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

		// �w��t���[���̃A�j���[�V�����������̂łƂ΂�
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
	/// �I�����Ă��邩���擾
	/// </summary>
	inline bool GetEnd()const
	{
		return end_;
	}

	/// <summary>
	/// ���[�v���Ă��邩���擾
	/// </summary>
	/// <returns></returns>
	inline bool GetLoop()const
	{
		return loop_;
	}

	/// <summary>
	/// �t�Đ����邩��ݒ�
	/// </summary>
	/// <param name="value">�t�Đ���</param>
	inline void SetReverse(const bool value)
	{
		reverse_ = value;
	}

	/// <summary>
	/// �t�Đ��ɂȂ��Ă��邩���擾
	/// </summary>
	inline bool GetReverse()const
	{
		return reverse_;
	}

	/// <summary>
	/// ���݂̕b���擾
	/// </summary>
	inline float GetTime()const
	{
		return time_;
	}

	/// <summary>
	/// �I��������
	/// </summary>
	inline void End()
	{
		time_ = keys_.rbegin()->first;
		end_ = true;
	}
};
