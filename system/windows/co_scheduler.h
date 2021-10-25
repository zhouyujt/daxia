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
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_COSCHEDULER_H
#define __DAXIA_SYSTEM_WINDOWS_COSCHEDULER_H

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include "coroutine.h"
#include "co_methods.h"
#include "../threadpool/thread_pool.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class CoScheduler
			{
			public:
				CoScheduler();
				~CoScheduler();
			public:
				std::shared_ptr<Coroutine> StartCoroutine(std::function<void(CoMethods& coMethods)>&& fiber);
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
				static long long nextId_;
				void* mainFiber_;
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_WINDOWS_COSCHEDULER_H
#endif // !_WIN32
