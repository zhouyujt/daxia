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
#ifndef __DAXIA_SYSTEM_WINDOWS_FILE_H
#define __DAXIA_SYSTEM_WINDOWS_FILE_H
#include <memory>
#include "../../string.hpp"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class File
			{
			public:
				enum Type
				{
					file,
					directory
				};
			public:
				File(const char* name, Type type = file);
				File(const wchar_t* name, Type type = file);
				~File();
			public:
				class iterator
				{
				protected:
					iterator();
					~iterator();
				public:
					iterator& operator++();
					bool operator==(const iterator& iter) const;
					bool operator!=(const iterator& iter) const;
					const std::shared_ptr<File> operator->() const;
					const std::shared_ptr<File> operator*() const;
					std::shared_ptr<File> operator->();
					std::shared_ptr<File> operator*();
					iterator& operator=(const iterator& iter);
				};

				class recursion_iterator
				{
				protected:
					recursion_iterator();
					~recursion_iterator();
				public:
					recursion_iterator& operator++();
					bool operator==(const recursion_iterator& iter) const;
					bool operator!=(const recursion_iterator& iter) const;
					const std::shared_ptr<File> operator->() const;
					const std::shared_ptr<File> operator*() const;
					std::shared_ptr<File> operator->();
					std::shared_ptr<File> operator*();
					recursion_iterator& operator=(const recursion_iterator& iter);
				};

				// STL������
			public:
				iterator begin();
				iterator end();
				iterator find(const char* name);
				iterator find(const wchar_t* name);

				recursion_iterator rbegin();
				recursion_iterator rend();
				recursion_iterator rfind(const char* name);
				recursion_iterator rfind(const wchar_t* name);

				// ����
			public:
				bool IsExists() const;
				size_t Size() const;

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
				bool Write(const daxia::buffer& buffer, size_t pos = 0, bool truncate = true);
			private:

			};
		}
	}
}
#endif	// !__DAXIA_SYSTEM_WINDOWS_FILE_H