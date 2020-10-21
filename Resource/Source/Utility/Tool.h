#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include <cassert>
#include "Utility/Constant.h"

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
		return path.substr(Size_t(idx) + 1, path.length() - idx - 1);
	}

	// 1�o�C�gstring��wstring�ɕϊ�����
	std::wstring WStringFromString(const std::string& str)
	{
		//�Ăяo��1���(�����񐔂𓾂�)
		auto wcharNum = MultiByteToWideChar(
			CP_ACP,
			0,
			str.c_str(), static_cast<int>(str.length()), nullptr, 0);

		std::wstring wstr;//string��wchar_t��
		wstr.resize(wcharNum);//����ꂽ�����񐔂Ń��T�C�Y

		//�Ăяo��2���(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		wcharNum = MultiByteToWideChar(
			CP_ACP,
			0,
			str.c_str(), -1, &wstr[0], static_cast<int>(wstr.size()));

		return wstr;
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
		auto folderPath = modelPath.substr(0, Size_t(pathIndex) + 1);
		return folderPath;
	}
}