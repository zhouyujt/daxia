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
				File(const char* path, Type type = file);
				File(const wchar_t* path, Type type = file);
				~File();
				// ����
			public:
				bool IsExists() const;
				Type FileType() const;
				size_t Size() const;
				const daxia::wstring Path() const;
				daxia::wstring Name() const;
				daxia::wstring Extension() const;
				const daxia::system::DateTime& CreateTime() const;
				const daxia::system::DateTime& AccessTime() const;
				const daxia::system::DateTime& WriteTime() const;

				// �ƶ������ơ�������ɾ��
			public:
				bool Move(const char* path) const;
				bool Move(const wchar_t* path) const;
				bool Copy(const char* path) const;
				bool Copy(const wchar_t* path) const;
				bool Create() const;
				bool Delete() const;

				// ����д
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
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FILE_H
#endif // !_WIN32