#pragma once
#include <string>

namespace
{
	constexpr unsigned int SHADOW_RESOLUTION = 2048;
	constexpr int FAILED = -1;
	const std::wstring D3D_SPACE_SCREEN = L"3d_space_screen";
}

#define H_ASSERT(X) assert(SUCCEEDED(X));

#define Size_t(X)	static_cast<size_t>(X)
#define Int(X)		static_cast<int>(X)
#define Uint(X)		static_cast<unsigned int>(X)
#define Float(X)	static_cast<float>(X)
#define Double(X)	static_cast<double>(X)