/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file find_file.h
* \author 漓江里的大虾
* \date 九月 2021
*
*  跨平台的文件、目录遍历查找
*
*/
#ifndef __DAXIA_SYSTEM_FIND_FILE_H
#define __DAXIA_SYSTEM_FIND_FILE_H
#include "windows/find_file.h"
#include "linux/find_file.h"
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