/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file access_token.h
* \author 漓江里的大虾
* \date 六月 2021
*
* WIN32 Access Token 相关操作
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_ACCESSTOKEN_H
#define __DAXIA_SYSTEM_WINDOWS_ACCESSTOKEN_H
#include "../../string.hpp"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class AccessToken
			{
			public:
				AccessToken(void* process);
				// 防止token_多次被CloseHandle，直接禁用拷贝构造函数
				AccessToken(const AccessToken&) = delete;
				~AccessToken();
			public:
				// 指定的权限是否开启
				bool IsPrivilegeEnabled(const char* privilege) const;
				bool IsPrivilegeEnabled(const wchar_t* privilege) const;

				// 开启/禁用(不会增加或删除)指定的权限
				bool EnablePrivilege(const char* privilege, bool enable) const;
				bool EnablePrivilege(const wchar_t* privilege, bool enable) const;

				void* GetHandle() const;

				// 获取所属域及用户名。例如： hello-world\\Administrator
				daxia::wstring GetUser() const;

				// 当前线程token模拟为该token
				bool Impersonate() const;
			public:
				operator bool() const{ return token_ != nullptr; }
			private:
				void* token_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_ACCESSTOKEN_H
#endif // !_WIN32

