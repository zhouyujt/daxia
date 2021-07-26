/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file access_token.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* WIN32 Access Token ��ز���
*
*/
#ifdef _MSC_VER
#ifndef _DAXIA_WIN32_ACCESSTOKEN_H
#define _DAXIA_WIN32_ACCESSTOKEN_H
#include <string>

namespace daxia
{
	namespace win32
	{
		class AccessToken
		{
		public:
			AccessToken(void* process);
			// ��ֹtoken_��α�CloseHandle��ֱ�ӽ��ÿ������캯��
			AccessToken(const AccessToken&) = delete;
			~AccessToken();
		public:
			// ָ����Ȩ���Ƿ���
			bool IsPrivilegeEnabled(const char* privilege) const;
			bool IsPrivilegeEnabled(const wchar_t* privilege) const;

			// ����/����(�������ӻ�ɾ��)ָ����Ȩ��
			bool EnablePrivilege(const char* privilege, bool enable) const;
			bool EnablePrivilege(const wchar_t* privilege, bool enable) const;

			void* GetHandle() const;

			// ��ȡ�������û��������磺 hello-world\\Administrator
			std::wstring GetUser() const;

			// ��ǰ�߳�tokenģ��Ϊ��token
			bool Impersonate() const;
		public:
			operator bool() const{ return token_ != nullptr; }
		private:
			void* token_;
		};
	}
}
#endif // !_DAXIA_WIN32_ACCESSTOKEN_H
#endif // !_MSC_VER

