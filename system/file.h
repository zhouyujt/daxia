/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file file.h
* \author 漓江里的大虾
* \date 九月 2021
*
*  跨平台的文件、目录相关的操作
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