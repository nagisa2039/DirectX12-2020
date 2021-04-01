#include "FPSManager.h"
#include <windows.h>
#include "Utility/Constant.h"
#include "Utility/Cast.h"

namespace
{
	// ƒ~ƒŠ•b”‚Å‹N“®ŠÔ‚ğæ“¾
	unsigned int GetTickCountHQ()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		double unit = 1000.0 / frequency.QuadPart;
		LARGE_INTEGER cnt;
		QueryPerformanceCounter(&cnt);
		return cnt.QuadPart * unit;
	}
}

FPSManager::FPSManager(const unsigned int fps): fixedFPS_(fps)
{
	startTime_ = 0;
	prevFrameStartTime_ = Uint32(GetTickCount64());
	deltaTime_ = 0.0f;
}

void FPSManager::Wait()
{
	if (fixedFPS_ <= 0)return;

	int targetTime = 1000 / fixedFPS_;

	int time = 0;
	do
	{
		time = Int32(GetTickCountHQ() - prevFrameStartTime_);
	} while (time < targetTime);

	auto tickCount = GetTickCountHQ();
	deltaTime_ = (tickCount - prevFrameStartTime_) / 1000.0f;

	prevFrameStartTime_ = Uint32(tickCount);
}

const unsigned int FPSManager::FixedFPS()const
{
	return fixedFPS_;
}

const float FPSManager::GetFPS()const
{
	return 1.0f / deltaTime_;
}

const float FPSManager::GetDeltaTime()const
{
	return deltaTime_;
}
