#pragma once
#include <map>
#include <vector>
#include <string>
#include "Utility/Geometry.h"

//class Camera;

class Animator
{
public:
	/// <summary>
	/// �A�j���[�V�����Đ��N���X
	/// </summary>
	Animator();
	~Animator() = default;

	struct AnimRect
	{
		int handle;
		Rect rect;
		Vector2i center;
	};

	struct Animation
	{
		std::vector<AnimRect> animRectVec;
		int itv = 0;
		bool loop = false;
	};

	void InitAnim(
		const std::wstring& animName, const std::vector<AnimRect>& animRectVec,
		const int itv, const bool loop);

	bool FolderImageRead(const std::wstring& path, std::vector<AnimRect>& animRect);

	bool FolderImageReadAndInitAnim(const std::wstring& animName, const int itv, const bool loop, const std::wstring& path);

	/// <summary>
	/// �A�j���[�V�����̕ύX
	/// </summary>
	/// <param name="animName">�A�j���[�V������</param>
	void ChengeAnim(const std::wstring& animName);

	/// <summary>
	/// �A�j���[�V�����I���󋵂̎擾
	/// </summary>
	/// <returns>true �I�� : false �Đ��� </returns>
	bool GetEnd()const;

	/// <summary>
	/// ���݂̃A�j���[�V�������̎擾
	/// </summary>
	const std::wstring& GetCurrentAnimName()const;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="pos">���S���W</param>
	/// <param name="exRate">�g�嗦</param>
	/// <param name="turn">���]</param>
	/// <param name="camera">�J����</param>
	void Draw(const Vector2i pos, const float exRate, const bool turn);

private:
	std::map<std::wstring, Animation> animations_;

	std::wstring animationName_;
	int animFrame_;
};