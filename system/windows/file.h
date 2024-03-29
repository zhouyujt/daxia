/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file file.h
* \author 漓江里的大虾
* \date 八月 2021
*
*  文件、目录相关的操作
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_FILE_H
#define __DAXIA_SYSTEM_WINDOWS_FILE_H
#include "../../string.hpp"
#include "../datetime.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class FindFile;
			class File
			{
				friend FindFile;
			public:
				enum Type
				{
					file = 0,
					directory
				};
			public:
				File(const char* path);
				File(const wchar_t* path);
				~File();
			private:
				File(const char* path, Type type);
				File(const wchar_t* path, Type type);
				// 属性
			public:
				Type FileType() const;
				size_t Size() const;
				const daxia::wstring Path() const;
				daxia::wstring Name() const;
				daxia::wstring Extension() const;
				const daxia::system::DateTime& CreateTime() const;
				const daxia::system::DateTime& AccessTime() const;
				const daxia::system::DateTime& WriteTime() const;

				// 移动、复制、创建、删除
			public:
				bool Move(const char* path) const;
				bool Move(const wchar_t* path) const;
				bool Copy(const char* path) const;
				bool Copy(const wchar_t* path) const;
				bool Delete() const;
				static bool IsExists(const char* path);
				static bool IsExists(const wchar_t* path);
				static File Create(const char* path, Type type);
				static File Create(const wchar_t* path, Type type);

				// 读、写
			public:
				bool Read(daxia::buffer& buffer, size_t pos = 0, size_t len = -1);
				bool Write(const daxia::buffer& buffer, bool truncate = true);
			private:
				daxia::wstring path_;
				Type type_;
				size_t size_;
				daxia::system::DateTime createTime_;
				daxia::system::DateTime accessTime_;
				daxia::system::DateTime writeTime_;
			private:
				void init(const wchar_t* path);
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FILE_H
#endif // !_WIN32