/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file coroutine_scheduler.h
* \author 漓江里的大虾
* \date 八月 2021
*
* 协程调度器
*
*/
#ifndef __DAXIA_COROUTINE_COROUTINE_SCHEDULER_H
#define __DAXIA_COROUTINE_COROUTINE_SCHEDULER_H

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include "coroutine.h"
#include "../system/threadpool/thread_pool.h"

namespace daxia
{
	namespace coroutine
	{
		class Scheduler
		{
		public:
			Scheduler();
			~Scheduler();
		public:
			std::shared_ptr<Coroutine> StartCoroutine(std::function<void()> fiber);
		private:
			void run();
			void addCoroutine(std::function<void()> fiber);
			void delCoroutine(std::shared_ptr<Coroutine> coroutine);
		private:
			daxia::system::ThreadPool threadPool_;
			std::list<std::shared_ptr<Coroutine>> coroutines_;
			std::mutex couroutinesLocker_;
		};
	}
}

#endif // !__DAXIA_COROUTINE_COROUTINE_SCHEDULER_H