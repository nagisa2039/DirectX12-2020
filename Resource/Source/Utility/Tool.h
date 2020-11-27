#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <cassert>
#include "Utility/Constant.h"
#include <algorithm>
#include "Utility/Cast.h"

namespace
{
	std::vector<std::string> SplitFileName(const std::string& path, const char splitter = '*')
	{
		std::vector<std::string> ret;
		size_t idx = 0;
		size_t offset = 0;
		do {
			idx = path.find(splitter, offset);
			if (idx != std::string::npos)
			{
				ret.emplace_back(path.substr(offset, idx - offset));
				offset = idx + 1;
			}
			else
			{
				ret.emplace_back(path.substr(offset));
			}
		} while (idx != std::string::npos);

		return ret;
	}

	std::string GetExtension(const std::string& path)
	{
		int idx = static_cast<int>(path.find_last_of('.'));
		return path.substr(Uint64(idx) + 1, path.length() - idx - 1);
	}

	std::wstring GetExtension(const std::wstring& path)
	{
		int idx = static_cast<int>(path.find_last_of('.'));
		return path.substr(Uint64(idx) + 1, path.length() - idx - 1);
	}

	// 1�o�C�gstring��wstring�ɕϊ�����
	std::wstring WStringFromString(const std::string& str)
	{
		std::wstring wstr;
		auto wsize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, 
			str.c_str(), Uint32(str.length()), nullptr, 0);

		wstr.resize(wsize);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), Uint32(str.length()), wstr.data(), Uint32(wstr.length()));

		return wstr;
	}

	// wstring��string�ɕϊ�
	std::string StringFromWString(const std::wstring& wstr)
	{
		// wstring �� SJIS
		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str()
			, -1, (char*)NULL, 0, NULL, NULL);

		// �o�b�t�@�̎擾
		CHAR* cpMultiByte = new CHAR[iBufferSize];

		// wstring �� SJIS
		WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, cpMultiByte
			, iBufferSize, NULL, NULL);

		// string�̐���
		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

		// �o�b�t�@�̔j��
		delete[] cpMultiByte;

		// �ϊ����ʂ�Ԃ�
		return(oRet);
	}

	UINT64 AlignmentValue(UINT64 size, UINT64 alignment)
	{
		if (size%alignment == 0)
		{
			return size;
		}
		return (size + alignment - (size%alignment));
	}

	std::string GetFolderPath(const std::string& modelPath)
	{
		int pathIndex1 = static_cast<int>(modelPath.rfind('/'));
		int pathIndex2 = static_cast<int>(modelPath.rfind('\\'));
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, Uint64(pathIndex) + 1);
		return folderPath;
	}

	template<class T>
	T Saturate(const T value, const T min = 0.0f, const T max = 1.0f)
	{
		return std::clamp(value, min, max);
	}
}