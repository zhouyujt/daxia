/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file co_scheduler.h
* \author 漓江里的大虾
* \date 十月 2021
*
* 协程调度器
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
				// 创建一个协程
				std::shared_ptr<Coroutine> StartCoroutine(std::function<void()> fiber, size_t stackSize = 1024 * 4/*设置协程栈大小，默认4KB。指定的数值将向上取整为4KB的倍数*/);
				// 停止所有协程
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
