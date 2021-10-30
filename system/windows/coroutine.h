/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file coroutine.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
* Э�̶���
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_COROUTINE_H
#define __DAXIA_SYSTEM_WINDOWS_COROUTINE_H
#include <functional>
#include <future>
#include "../datetime.h"
#include "this_coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class CoScheduler;
			class Coroutine
			{
				friend CoScheduler;
				friend void daxia::system::windows::this_coroutine::CoSleep(size_t milliseconds);
				friend void daxia::system::windows::this_coroutine::CoYield();
				friend void daxia::system::windows::this_coroutine::CoWait(std::function<bool()>&& wakeupCondition);
			protected:
				Coroutine(std::function<void()>&& fiber, size_t stackSize, long long id, void** mainFiber);
			public:
				~Coroutine();
			public:
				// ��ȡЭ��ID
				long long Id() const { return id_; }
				// �ȴ�Э��ִ�����
				void Join();
				// ǿ�ƽ���Э�̣���Э������ִ��CoMethods��ط���ʱǿ�ƽ�����
				void Terminate();
			private:
				// Э��ID
				long long id_;
				// �����ѣ�ִ�У��Ĵ���
				long long wakeupCount_;
				// ִ����ϱ�־
				bool complete_;
				// ִ������¼�����
				HANDLE completeEvent_;
				// Э����
				std::function<void()> fiberStartRoutine_;
				// WIN32 API CreateFiber ���ص�Э�̵�ַ
				void* fiber_;
				// ��Э�̵�ַ(WIN32 API CreateFiber ����)
				void** mainFiber_;
			private:
				// ǿ�ƽ�����־
				bool terminate_;
				// ˯��ʱ��
				size_t sleepMilliseconds_;
				// ˯��ʱ���
				daxia::system::DateTime sleepTimestamp_;
				// �����־
				bool yield_;
				// ��������
				std::function<bool()> wakeupCondition_;
			private:
				static void WINAPI fiberStartRoutine(LPVOID param);
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_WINDOWS_COROUTINE_H
#endif // !_WIN32