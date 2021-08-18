/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file process.h
* \author 漓江里的大虾
* \date 六月 2021
*
* WIN32 进程相关的操作
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
				// 防止handle_多次被CloseHandle，直接禁用拷贝构造函数
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
				// 杀死进程
				bool Kill();
				// 获取进程ID
				unsigned long GetId() const;
				// 获取进程句柄
				void* GetHandle() const;
				// 获取进程名
				daxia::wstring GetName() const;
				// 获取进程执行文件路径
				daxia::wstring GetPath() const;
				// 获取进程当前环境目录
				daxia::wstring GetDirectory() const;
				// 获取AccessToken
				std::shared_ptr<AccessToken> GetAccessToken();
				// 获取Modules
				const std::shared_ptr<Modules> GetModules() const;
				// 从内存中加载Dll
				void* LoadMemLibrary(const char* data, size_t len) const;
				FARPROC GetMemProcAddress(void* address, const char* name) const;
				FARPROC GetMemProcAddress(void* address, int order) const;
				void FreeMemLibrary(void* address) const;
			public:
				// 恢复为自身的token
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

