/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file find_file.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
*  ��ƽ̨���ļ���Ŀ¼��������
*
*/
#ifndef __DAXIA_SYSTEM_FIND_FILE_H
#define __DAXIA_SYSTEM_FIND_FILE_H
#ifdef _WIN32
#include "windows/find_file.h"
#else
#include "linux/find_file.h"
#endif
namespace daxia
{
	namespace system
	{
#ifdef _WIN32
		using FindFile = daxia::system::windows::FindFile;
#else
		using FindFile = daxia::system::linux::FindFile;
#endif
	}
}
#endif // !__DAXIA_SYSTEM_FIND_FILE_H