/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file co_scheduler.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
* Э�̵�����
*
*/
#ifdef __linux__
#ifndef __DAXIA_SYSTEM_LINUX_COSCHEDULER_H
#define __DAXIA_SYSTEM_LINUX_COSCHEDULER_H

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include <ucontext.h>
#include <pthread.h>
#include "coroutine.h"
#include "this_coroutine.h"
#include "../threadpool/thread_pool.h"

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			class CoScheduler
			{
				friend void daxia::system::linux::this_coroutine::CoSleep(size_t milliseconds);
				friend void daxia::system::linux::this_coroutine::CoYield();
				friend void daxia::system::linux::this_coroutine::CoWait(std::function<bool()>&& wakeupCondition);
			public:
				CoScheduler();
				~CoScheduler();
			public:
				// ����һ��Э��
				std::shared_ptr<Coroutine> StartCoroutine(std::function<void()> fiber, size_t stackSize = 1024 * 4/*����Э��ջ��С��Ĭ��4KB��ָ������ֵ������ȡ��Ϊ4KB�ı���*/);
				// ֹͣ����Э��
				void Stop();
			private:
				void run();
				void addCoroutine(std::shared_ptr<Coroutine> coroutine);
				long long makeCoroutineId();
			private:
				daxia::system::ThreadPool threadPool_;
				std::list<std::shared_ptr<Coroutine>> coroutines_;
				std::mutex couroutinesLocker_;
				std::mutex idLocker_;
				bool run_;
				ucontext_t mainCtx_;
				static long long nextId_;
				static pthread_key_t currCoroutineKey_;
				static std::once_flag onceCreateKeyFlag_;
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_LINUX_COSCHEDULER_H
#endif // !__linux__
