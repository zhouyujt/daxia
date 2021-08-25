/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file file.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
*  �ļ���Ŀ¼��صĲ���
*
*/
#ifdef _MSC_VER
#ifndef __DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#define __DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#include <memory>
#include "file.h"
#include "../../string.hpp"
namespace daxia
{
	namespace system
	{
		namespace windows
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
					iterator(std::shared_ptr<void> handle, std::shared_ptr<File> file);
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
					std::shared_ptr<void> handle_;
					std::shared_ptr<File> file_;
				};

				// STL������
			public:
				iterator begin();
				iterator end();
				iterator find(const char* name);
				iterator find(const wchar_t* name);
			private:
				daxia::wstring path_;
				daxia::wstring name_;
				bool recursion_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FIND_FILE_H
#endif // !_MSC_VER