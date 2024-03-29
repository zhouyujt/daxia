/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file find_file.h
* \author 漓江里的大虾
* \date 九月 2021
*
*  文件、目录遍历查找
*
*/
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#define __DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#include <memory>
#include <dirent.h>
#include "file.h"
#include "../../string.hpp"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			class FindFile
			{
			public:
				FindFile(const char* path, const char* name = "*.*", bool recursion = false);
				FindFile(const wchar_t* path, const wchar_t* name = L"*.*", bool recursion = false);
				~FindFile();
			public:
				class iterator
				{
					friend FindFile;
				public:
					iterator();
					~iterator();
				private:
					iterator(std::shared_ptr<DIR> handle, std::shared_ptr<File> file);
				public:
					iterator& operator++();
					bool operator==(const iterator& iter) const;
					bool operator!=(const iterator& iter) const;
					const std::shared_ptr<File> operator->() const;
					const std::shared_ptr<File> operator*() const;
					std::shared_ptr<File> operator->();
					std::shared_ptr<File> operator*();
					iterator& operator=(const iterator& iter);
				private:
					std::shared_ptr<DIR> handle_;
					std::shared_ptr<File> file_;
				};

				// STL风格遍历
			public:
				iterator begin();
				iterator end();
				iterator find(const char* name);
				iterator find(const wchar_t* name);
			private:
				daxia::string path_;
				daxia::string name_;
				bool recursion_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#endif // !__linux__