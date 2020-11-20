#pragma once
#include <string>

namespace
{
	constexpr int FAILED = -1;
	constexpr unsigned int SHADOW_RESOLUTION = 2048;
	const std::wstring D3D_CAMERA_MR_COLOR		= L"3d_camera_mr_color";
	const std::wstring D3D_CAMERA_MR_NORMAL		= L"3d_camera_mr_normal";
	const std::wstring D3D_CAMERA_MR_BRIGHT		= L"3d_camera_mr_bright";
	const std::wstring D3D_CAMERA_VIEW_SCREEN	= L"3d_camera_view_screen";
	const std::wstring D3D_CAMERA_SHRINK_SCREEN	= L"3d_camera_shrink_screen";
}

#define H_ASSERT(X) assert(SUCCEEDED(X))