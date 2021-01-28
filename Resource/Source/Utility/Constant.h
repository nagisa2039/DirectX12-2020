#pragma once
#include <string>

// 使用定数
namespace
{
	// 失敗
	constexpr int FAILED = -1;
	// 影描画先スクリーンサイズ
	constexpr unsigned int SHADOW_RESOLUTION = 2048;

	// 3D描画時の色描画先スクリーン名
	const std::wstring SCR_CAMERA_MR_COLOR		= L"scr_camera_mr_color";
	// 3D描画時の法線描画先スクリーン名
	const std::wstring SCR_CAMERA_MR_NORMAL		= L"scr_camera_mr_normal";
	// 3D描画時の高輝度描画先スクリーン名
	const std::wstring SCR_CAMERA_MR_BRIGHT		= L"scr_camera_mr_bright";
	// 3D描画最終出力スクリーン名
	const std::wstring SCR_CAMERA_VIEW_SCREEN	= L"scr_camera_view_screen";
	// 3D描画時の色の縮小スクリーン名
	const std::wstring SCR_COLOR_SHRINK			= L"scr_color_shrink_screen";
	// 3D描画時の高輝度の縮小スクリーン名
	const std::wstring SCR_EMMISION_SHRINK		= L"scr_emmision_shrink_screen";
}

// HRESULTのassertチェック
#define H_ASSERT(X) assert(SUCCEEDED(X))