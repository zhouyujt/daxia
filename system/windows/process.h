/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file process.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* WIN32 ������صĲ���
*
*/
#ifdef _MSC_VER
#ifndef __DAXIA_SYSTEM_WINDOWS_PROCESS_H
#define __DAXIA_SYSTEM_WINDOWS_PROCESS_H

#include <memory>
#include <vector>
#include <tlhelp32.h>
#include "../../string.hpp"
#include "access_token.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class Process
			{
			public:
				Process();
				Process(unsigned long id);
				// ��ֹhandle_��α�CloseHandle��ֱ�ӽ��ÿ������캯��
				Process(const Process&) = delete;
				~Process();
			public:
				class Modules
				{
					friend Process;
				private:
					Modules(unsigned long pid);
				public:
					class iterator
					{
						friend Modules;
					public:
						iterator();
					private:
						iterator(std::shared_ptr<void> handle, const MODULEENTRY32& me);
					public:
						iterator& operator++();
						bool operator==(const iterator& iter) const;
						bool operator!=(const iterator& iter) const;
						const tagMODULEENTRY32W*  operator->() const;
						const tagMODULEENTRY32W& operator*() const;
						tagMODULEENTRY32W*  operator->();
						tagMODULEENTRY32W& operator*();
						iterator& operator=(const iterator& iter);
					private:
						std::shared_ptr<void> handle_;
						tagMODULEENTRY32W me_;
					};
				public:
					iterator begin() const;
					iterator end() const;
					iterator find(const char* name, const iterator& pos) const;
					iterator find(const wchar_t* name, const iterator& pos) const;
				private:
					unsigned long pid_;
				};

				class Threads
				{
					friend Process;
				private:
					Threads();
				public:
					class iterator
					{

					};
				public:
					iterator begin();
					iterator end();
					iterator find();
				};
			public:
				// ɱ������
				bool Kill();
				// ��ȡ����ID
				unsigned long GetId() const;
				// ��ȡ���̾��
				void* GetHandle() const;
				// ��ȡ������
				daxia::wstring GetName() const;
				// ��ȡ����ִ���ļ�·��
				daxia::wstring GetPath() const;
				// ��ȡ���̵�ǰ����Ŀ¼
				daxia::wstring GetDirectory() const;
				// ��ȡAccessToken
				std::shared_ptr<AccessToken> GetAccessToken();
				// ��ȡModules
				const std::shared_ptr<Modules> GetModules() const;
				// ���ڴ��м���Dll
				void* LoadMemLibrary(const char* data, size_t len) const;
				FARPROC GetMemProcAddress(void* address, const char* name) const;
				FARPROC GetMemProcAddress(void* address, int order) const;
				void FreeMemLibrary(void* address) const;
			public:
				// �ָ�Ϊ�����token
				static bool RevertToSelf();
			public:
				operator bool() const{ return handle_ != nullptr; }
			private:
				unsigned long getImageSize(const char* imageData) const;
				void mapImage(const char* imageData, size_t len, char* address, size_t size) const;
				void adjustRelocation(char* address) const;
				void adjustImport(char* address) const;
				void setImageBase(char* address) const;
				void callDllMain(char* address, int reason) const;
			private:
				void* handle_;
				unsigned long id_;
				std::shared_ptr<AccessToken> token_;
				daxia::wstring name_;
				daxia::wstring path_;
				std::shared_ptr<Modules> modules_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_PROCESS_H
#endif // !_MSC_VER

