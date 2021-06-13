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
			// �����༶Ŀ¼
			static bool CreateDirectory(const char* dir);
			static bool CreateDirectory(const wchar_t* dir);

			// ����csidl(https://docs.microsoft.com/en-us/windows/win32/shell/csidl)���ָ����·��
			static std::wstring GetSpecialPath(int csidl/*e.g. CSIDL_APPDATA*/,bool create = false);

			// �����ļ���
			static std::wstring FindFileName(const wchar_t* dir);
			// ������չ��	
			static std::wstring FindExtension(const wchar_t* dir);
		};
	}
}
#endif // !_DAXIA_WIN32_PATH_H

