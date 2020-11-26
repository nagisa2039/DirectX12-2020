#include "FPSManager.h"
#include <windows.h>
#include "Utility/Constant.h"
#include "Utility/Cast.h"

FPSManager::FPSManager(const int fps): fixedFPS_(fps)
{
	startTime_ = Uint32(GetTickCount64());
	prevFrameStartTime_ = 0;
	deltaTime_ = 0.0f;
}

void FPSManager::Wait()
{
	auto tickCount = GetTickCount64();
	int elapsedTime = Int32(tickCount - prevFrameStartTime_);
	int targetTime = 1000 / fixedFPS_;
	Sleep(max(targetTime - elapsedTime,0));
	deltaTime_ = (tickCount - prevFrameStartTime_) / 1000.0f;
	prevFrameStartTime_ = GetTickCount64();
}

int FPSManager::FixedFPS()
{
	return fixedFPS_;
}

float FPSManager::GetFPS()
{
	return 1.0f / deltaTime_;
}

float FPSManager::GetDeltaTime()
{
	return deltaTime_;
}
