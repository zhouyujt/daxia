/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file window.h
* \author 漓江里的大虾
* \date 十一月 2021
*
*  窗口相关的操作
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_WINDOW_H
#define __DAXIA_SYSTEM_WINDOWS_WINDOW_H
#include <windows.h>
#include <memory>
#include "../../string.hpp"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class Window
			{
			public:
				Window(HWND handle);
				~Window();
			public:
				HWND GetHandle() const;
				bool IsVisable() const;
				void Print(HDC hdcBlt) const;
				void Bitblt(HDC hdcBlt) const;
			private:
				HWND handle_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_WINDOW_H
#endif // !_WIN32