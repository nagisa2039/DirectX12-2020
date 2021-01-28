#pragma once
#include <string>

// �g�p�萔
namespace
{
	// ���s
	constexpr int FAILED = -1;
	// �e�`���X�N���[���T�C�Y
	constexpr unsigned int SHADOW_RESOLUTION = 2048;

	// 3D�`�掞�̐F�`���X�N���[����
	const std::wstring SCR_CAMERA_MR_COLOR		= L"scr_camera_mr_color";
	// 3D�`�掞�̖@���`���X�N���[����
	const std::wstring SCR_CAMERA_MR_NORMAL		= L"scr_camera_mr_normal";
	// 3D�`�掞�̍��P�x�`���X�N���[����
	const std::wstring SCR_CAMERA_MR_BRIGHT		= L"scr_camera_mr_bright";
	// 3D�`��ŏI�o�̓X�N���[����
	const std::wstring SCR_CAMERA_VIEW_SCREEN	= L"scr_camera_view_screen";
	// 3D�`�掞�̐F�̏k���X�N���[����
	const std::wstring SCR_COLOR_SHRINK			= L"scr_color_shrink_screen";
	// 3D�`�掞�̍��P�x�̏k���X�N���[����
	const std::wstring SCR_EMMISION_SHRINK		= L"scr_emmision_shrink_screen";
}

// HRESULT��assert�`�F�b�N
#define H_ASSERT(X) assert(SUCCEEDED(X))