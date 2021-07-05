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
#ifndef _DAXIA_WIN32_PROCESS_H
#define _DAXIA_WIN32_PROCESS_H

#include <memory>
#include <string>
#include "access_token.h"

namespace daxia
{
	namespace win32
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
			// 杀死进程
			bool Kill();
			// 获取进程ID
			unsigned long GetId() const;
			// 获取进程句柄
			void* GetHandle() const;
			// 获取进程名
			std::wstring GetName() const;
			// 获取进程执行文件路径
			std::wstring GetPath() const;
			// 获取进程当前环境目录
			std::wstring GetDirectory() const;
			// 获取AccessToken
			std::shared_ptr<AccessToken> GetAccessToken();
			// 从内存中加载Dll
			char* LoadMemLibrary(const char* data, unsigned long len);
		public:
			// 恢复为自身的token
			static bool RevertToSelf();
		public:
			operator bool() const{ return handle_ != nullptr; }
		private:
			unsigned long getImageSize(const char* imageData) const;
			void mapImage(const char* imageData, unsigned long len, char* address, unsigned long size) const;
			void adjustRelocation(char* address);
			void adjustImport(char* address);
			void setImageBase(char* address);
			void callDllMain(char* address);
		private:
			void* handle_;
			std::shared_ptr<AccessToken> token_;
		};
	}
}
#endif // !_DAXIA_WIN32_PROCESS_H

