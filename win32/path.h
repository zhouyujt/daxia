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
#ifndef _DAXIA_WIN32_PATH_H
#define _DAXIA_WIN32_PATH_H

#include <string>
#include <ShlObj.h>

namespace daxia
{
	namespace win32
	{
		class Path
		{
		public:
			Path() = delete;
			Path(const Path&) = delete;
			~Path();
		public:
			// 创建多级目录
			static bool CreateDirectory(const char* dir);
			static bool CreateDirectory(const wchar_t* dir);

			// 根据csidl(https://docs.microsoft.com/en-us/windows/win32/shell/csidl)获得指定的路径
			static std::wstring GetSpecialPath(int csidl/*e.g. CSIDL_APPDATA*/,bool create = false);

			// 查找文件名
			static std::wstring FindFileName(const wchar_t* dir);
			// 查找扩展名	
			static std::wstring FindExtension(const wchar_t* dir);
		};
	}
}
#endif // !_DAXIA_WIN32_PATH_H

