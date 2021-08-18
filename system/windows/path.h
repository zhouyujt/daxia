/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file path.h
* \author 漓江里的大虾
* \date 六月 2021
*
* WIN32 文件路径相关操作
*
*/
#ifdef _MSC_VER
#ifndef __DAXIA_SYSTEM_WINDOWS_PATH_H
#define __DAXIA_SYSTEM_WINDOWS_PATH_H

#include <string>
#include <ShlObj.h>
#include "../../string.hpp"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class Path
			{
			public:
				Path() = delete;
				Path(const Path&) = delete;
				~Path();
			public:
				// 创建多级目录
				static bool CreateDirectory(const daxia::wstring& dir);
				// 根据csidl(https://docs.microsoft.com/en-us/windows/win32/shell/csidl)获得指定的路径
				static daxia::wstring GetSpecialPath(int csidl/*例如： CSIDL_APPDATA*/, bool create = false);
				// 获取临时目录
				static daxia::wstring GetTempPath();
				// 获取一个可用的临时文件路径
				static daxia::wstring GetTempFilePath();
				// 查找文件名
				static daxia::wstring FindFileName(const daxia::wstring& dir);
				// 查找扩展名	
				static daxia::wstring FindExtension(const daxia::wstring& dir);
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_PATH_H
#endif // !_MSC_VER

