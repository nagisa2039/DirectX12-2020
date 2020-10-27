#pragma once
#include <string>

namespace
{
	constexpr unsigned int SHADOW_RESOLUTION = 2048;
	constexpr int FAILED = -1;
	const std::wstring D3D_SPACE_SCREEN = L"3d_space_screen";
}

#define H_ASSERT(X) assert(SUCCEEDED(X));