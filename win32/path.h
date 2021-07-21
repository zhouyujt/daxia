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
#include "../string.hpp"

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
			static bool CreateDirectory(const daxia::tstring& dir);

			// 根据csidl(https://docs.microsoft.com/en-us/windows/win32/shell/csidl)获得指定的路径
			static daxia::tstring GetSpecialPath(int csidl/*例如： CSIDL_APPDATA*/, bool create = false);

			// 查找文件名
			static daxia::tstring FindFileName(const daxia::tstring& dir);
			// 查找扩展名	
			static daxia::tstring FindExtension(const daxia::tstring& dir);
		};
	}
}
#endif // !_DAXIA_WIN32_PATH_H

