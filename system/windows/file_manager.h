/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file file_manager.h
* \author 漓江里的大虾
* \date 八月 2021
*
* 文件、目录相关的操作
*
*/
#ifndef __DAXIA_SYSTEM_WINDOWS_FILEMANAGER_H
#define __DAXIA_SYSTEM_WINDOWS_FILEMANAGER_H
#include <memory>
#include "file.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class FileManager
			{
			protected:
				FileManager();
				~FileManager();
				// STL风格遍历
			public:
			};
		}
	}
}
#endif	// !__DAXIA_SYSTEM_WINDOWS_FILEMANAGER_H