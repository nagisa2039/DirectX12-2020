#include "FPSManager.h"
#include <windows.h>

FPSManager::FPSManager(const int fps): _fixedFPS(fps)
{
	_startTime = GetTickCount();
	_prevFrameStartTime = 0;
	_deltaTime = 0.0f;
}

void FPSManager::Wait()
{
	int wait = GetTickCount() - _prevFrameStartTime;
	int targetTime = 1000 / _fixedFPS;
	Sleep(max(targetTime - wait,0));
	wait = GetTickCount() - _prevFrameStartTime;
	_prevFrameStartTime += wait;
	_deltaTime = wait / 1000.0f;
}

int FPSManager::FixedFPS()
{
	return _fixedFPS;
}

float FPSManager::GetFPS()
{
	return 1.0f / _deltaTime;
}
