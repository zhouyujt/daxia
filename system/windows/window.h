/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file window.h
* \author �콭��Ĵ�Ϻ
* \date ʮһ�� 2021
*
*  ������صĲ���
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