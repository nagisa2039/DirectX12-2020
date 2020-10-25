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
	int wait = Int32(GetTickCount64() - prevFrameStartTime_);
	int targetTime = 1000 / fixedFPS_;
	Sleep(max(targetTime - wait,0));
	wait = Int32(GetTickCount64() - prevFrameStartTime_);
	prevFrameStartTime_ += wait;
	deltaTime_ = wait / 1000.0f;
}

int FPSManager::FixedFPS()
{
	return fixedFPS_;
}

float FPSManager::GetFPS()
{
	return 1.0f / deltaTime_;
}
