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
	/// アニメーション再生クラス
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
	/// アニメーションの変更
	/// </summary>
	/// <param name="animName">アニメーション名</param>
	void ChengeAnim(const std::wstring& animName);

	/// <summary>
	/// アニメーション終了状況の取得
	/// </summary>
	/// <returns>true 終了 : false 再生中 </returns>
	bool GetEnd()const;

	/// <summary>
	/// 現在のアニメーション名の取得
	/// </summary>
	const std::wstring& GetCurrentAnimName()const;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="pos">中心座標</param>
	/// <param name="exRate">拡大率</param>
	/// <param name="turn">反転</param>
	/// <param name="camera">カメラ</param>
	void Draw(const Vector2i pos, const float exRate, const bool turn);

private:
	std::map<std::wstring, Animation> animations_;

	std::wstring animationName_;
	int animFrame_;
};