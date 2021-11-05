/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file file.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
*  ��ƽ̨���ļ���Ŀ¼��صĲ���
*
*/
#ifndef __DAXIA_SYSTEM_FILE_H
#define __DAXIA_SYSTEM_FILE_H
#ifdef _WIN32
#include "windows/file.h"
#else
#include "linux/file.h"
#endif
namespace daxia
{
	namespace system
	{
#ifdef _WIN32
		using File = daxia::system::windows::File;
#else
		using File = daxia::system::linux::File;
#endif
	}
}

#endif // !__DAXIA_SYSTEM_FILE_H