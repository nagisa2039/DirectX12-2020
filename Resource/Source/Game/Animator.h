#pragma once
#include <map>
#include <vector>
#include <string>
#include <DirectXMath.h>

class Animator
{
public:
	/// <summary>
	/// アニメーション再生クラス
	/// </summary>
	Animator();
	~Animator() = default;

	struct Rect
	{
		DirectX::XMINT2 center;
		DirectX::XMINT2 size;

		unsigned int Top()const;
		unsigned int Bottom()const;
		unsigned int Left()const;
		unsigned int Right()const;
	};

	struct AnimRect
	{
		int handle;
		Rect rect;
		DirectX::XMINT2 center;
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
	void Draw(const DirectX::XMINT2& pos, const float exRate, const bool turn);

private:
	std::map<std::wstring, Animation> animations_;

	std::wstring animationName_;
	int animFrame_;
};