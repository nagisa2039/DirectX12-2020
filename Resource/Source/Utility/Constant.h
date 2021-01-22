#pragma once
#include <string>

namespace
{
	constexpr int FAILED = -1;
	constexpr unsigned int SHADOW_RESOLUTION = 2048;
	const std::wstring SCR_CAMERA_MR_COLOR		= L"scr_camera_mr_color";
	const std::wstring SCR_CAMERA_MR_NORMAL		= L"scr_camera_mr_normal";
	const std::wstring SCR_CAMERA_MR_BRIGHT		= L"scr_camera_mr_bright";
	const std::wstring SCR_CAMERA_VIEW_SCREEN	= L"scr_camera_view_screen";
	const std::wstring SCR_COLOR_SHRINK			= L"scr_color_shrink_screen";
	const std::wstring SCR_EMMISION_SHRINK		= L"scr_emmision_shrink_screen";
}

#define H_ASSERT(X) assert(SUCCEEDED(X))