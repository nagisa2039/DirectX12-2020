#include "FPSManager.h"
#include <windows.h>
#include "Utility/Constant.h"
#include "Utility/Cast.h"

FPSManager::FPSManager(const int fps): fixedFPS_(fps)
{
	startTime_ = 0;
	prevFrameStartTime_ = Uint32(GetTickCount64());
	deltaTime_ = 0.0f;
}

void FPSManager::Wait()
{
	auto tickCount = GetTickCount64();
	int time = Int32(tickCount - prevFrameStartTime_);
	int targetTime = 1000 / fixedFPS_;

	if (time < targetTime)
	{
		Sleep(targetTime - time);
	}

	tickCount = GetTickCount64();
	deltaTime_ = (tickCount - prevFrameStartTime_) / 1000.0f;

	prevFrameStartTime_ = Uint32(GetTickCount64());

//#ifdef _DEBUG
//	char buf[256];
//	sprintf_s(buf, 256, "%f\n", GetFPS());
//	OutputDebugString(buf);
//#endif
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
