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
				// ɱ������
				bool Kill();
				// ��ȡ����ID
				unsigned long GetId() const;
				// ��ȡ���̾��
				void* GetHandle() const;
				// ��ȡ������
				daxia::tstring GetName() const;
				// ��ȡ����ִ���ļ�·��
				daxia::tstring GetPath() const;
				// ��ȡ���̵�ǰ����Ŀ¼
				daxia::tstring GetDirectory() const;
				// ��ȡAccessToken
				std::shared_ptr<AccessToken> GetAccessToken();
				// ���ڴ��м���Dll
				void* LoadMemLibrary(const char* data, size_t len) const;
				FARPROC GetMemProcAddress(void* address, const char* name) const;
				FARPROC GetMemProcAddress(void* address, int order) const;
				void FreeMemLibrary(void* address) const;
			public:
				// �ָ�Ϊ������token
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
				std::shared_ptr<AccessToken> token_;
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_PROCESS_H
#endif // !_MSC_VER
