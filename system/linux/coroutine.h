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
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_LINUX_COROUTINE_H
#define __DAXIA_SYSTEM_LINUX_COROUTINE_H
#include <functional>
#include <future>
#include <semaphore.h>
#include <ucontext.h>
#include "../datetime.h"
#include "this_coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			class CoScheduler;

			class Coroutine
			{
				friend CoScheduler;
				friend void daxia::system::linux::this_coroutine::CoSleep(size_t milliseconds);
				friend void daxia::system::linux::this_coroutine::CoYield();
				friend void daxia::system::linux::this_coroutine::CoWait(std::function<bool()>&& wakeupCondition);
			protected:
				Coroutine(std::function<void()> fiber, size_t stackSize, long long id, ucontext_t* mainFiber);
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
				sem_t completeEvent_;
				// Э����
				std::function<void()> fiberStartRoutine_;
				// ��ǰЭ�̻���
				ucontext_t ctx_;
				// ��Э�̻���
				ucontext_t* mainCtx_;
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
				std::shared_ptr<char> stack_;
				static void fiberStartRoutine(Coroutine* co);
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_COROUTINE_H
#endif // !__linux__