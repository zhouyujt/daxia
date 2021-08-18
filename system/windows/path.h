/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file path.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* WIN32 �ļ�·����ز���
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
				// �����༶Ŀ¼
				static bool CreateDirectory(const daxia::wstring& dir);
				// ����csidl(https://docs.microsoft.com/en-us/windows/win32/shell/csidl)���ָ����·��
				static daxia::wstring GetSpecialPath(int csidl/*���磺 CSIDL_APPDATA*/, bool create = false);
				// ��ȡ��ʱĿ¼
				static daxia::wstring GetTempPath();
				// ��ȡһ�����õ���ʱ�ļ�·��
				static daxia::wstring GetTempFilePath();
				// �����ļ���
				static daxia::wstring FindFileName(const daxia::wstring& dir);
				// ������չ��	
				static daxia::wstring FindExtension(const daxia::wstring& dir);
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_PATH_H
#endif // !_MSC_VER

