#include "Animator.h"
#include "Utility/Cast.h"
#include "System/Application.h"
#include "2D/SpriteDrawer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include <sstream>
#include "Utility/Constant.h"

using namespace DirectX;

Animator::Animator()
{
	animationName_ = L"";
	animFrame_ = 0;
}

void Animator::InitAnim(const std::wstring& animName, const std::vector<AnimRect>& animRectVec, const int itv, const bool loop)
{
	animations_[animName].animRectVec = animRectVec;
	animations_[animName].itv = itv;
	animations_[animName].loop = loop;
}

bool Animator::FolderImageRead(const std::wstring& path, std::vector<AnimRect>& animRect)
{
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();

	int num = 0;
	
	animRect.clear();
	while (true)
	{
		std::wstringstream ss;
		ss << path.c_str() << num << L".png";
		auto handle = texLoader.LoadGraph(ss.str());
		if (handle == FAILED)
		{
			break;
		}

		unsigned int width, height;
		texLoader.GetGraphSize(handle, width, height);
		animRect.emplace_back(AnimRect{ handle, 
			Rect{XMINT2(width / 2, height / 2), XMINT2(width, height)}, XMINT2(width / 2, height - 1) });
		num++;
	}
	return num <= 0;
}

bool Animator::FolderImageReadAndInitAnim(const std::wstring& animName, const int itv, const bool loop, const std::wstring& path)
{
	std::vector<AnimRect> animRect;
	bool ret = FolderImageRead(path, animRect);
	InitAnim(animName, animRect, itv, loop);
	return ret;
}

void Animator::ChengeAnim(const std::wstring& animName)
{
	animationName_ = animName;
	animFrame_ = 0;
}

bool Animator::GetEnd() const
{
	assert(animations_.find(animationName_) != animations_.end());

	auto anim = animations_.at(animationName_);
	return animFrame_ >= anim.animRectVec.size() * anim.itv;
}

const std::wstring& Animator::GetCurrentAnimName() const
{
	return animationName_;
}

void Animator::Update()
{
	animFrame_++;
}

void Animator::Draw(const XMINT2& pos, const float exRate, const bool turn)
{
	const auto& anim = animations_[animationName_];
	int idx = 0;
	if (anim.loop)
	{
		idx = (animFrame_ / anim.itv) % anim.animRectVec.size();
	}
	else
	{
		idx = min(animFrame_ / anim.itv, Int32(anim.animRectVec.size()) - 1);
	}
	auto& animRect = anim.animRectVec[idx];

	auto drawRect = Rect{ pos, 
		XMINT2(Int32(animRect.rect.size.x * exRate), Int32(animRect.rect.size.y * exRate)) };
	//if (!camera.GetCameraRect().IsHit(drawRect))return;
	//drawRect.center += camera.GetOffset();

	auto& spriteDrawer = Application::Instance().GetDx12().GetSpriteDrawer();
	spriteDrawer.DrawRectRotaGraph2(drawRect.center.x, drawRect.center.y,
		animRect.rect.Left(), animRect.rect.Top(), animRect.rect.size.x, animRect.rect.size.y,
		animRect.center.x, animRect.center.y, exRate, 0.0, animRect.handle);
}

unsigned int Animator::Rect::Top()const
{
	return center.y - size.y / 2;
}
unsigned int Animator::Rect::Bottom()const
{
	return center.y + size.y / 2;
}
unsigned int Animator::Rect::Left()const
{
	return center.x - size.x / 2;
}
unsigned int Animator::Rect::Right()const
{
	return center.x + size.x / 2;
}