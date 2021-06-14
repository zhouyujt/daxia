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
			std::wstring GetName() const;
			// ��ȡ����ִ���ļ�·��
			std::wstring GetPath() const;
			// ��ȡ���̵�ǰ����Ŀ¼
			std::wstring GetDirectory() const;
			// ��ȡAccessToken
			std::shared_ptr<AccessToken> GetAccessToken();
		public:
			// �ָ�Ϊ������token
			static bool RevertToSelf();
		public:
			operator bool() const{ return handle_ != nullptr; }
		private:
			void* handle_;
			std::shared_ptr<AccessToken> token_;
		};
	}
}
#endif // !_DAXIA_WIN32_PROCESS_H
