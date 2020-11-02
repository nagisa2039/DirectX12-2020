#pragma once
#include <string>

namespace
{
	constexpr int FAILED = -1;
	constexpr unsigned int SHADOW_RESOLUTION = 2048;
	const std::wstring D3D_CAMERA_VIEW_SCREEN = L"3d_camera_view_screen";
	const std::wstring D3D_LIGHT_VIEW_SCREEN = L"3d_light_view_screen";
}

#define H_ASSERT(X) assert(SUCCEEDED(X))