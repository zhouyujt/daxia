/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file coroutine_scheduler.h
* \author �콭��Ĵ�Ϻ
* \date ʮ�� 2021
*
* Э�̵�����
*
*/
#ifdef _WIN32
#ifndef __DAXIA_COROUTINE_COROUTINE_SCHEDULER_H
#define __DAXIA_COROUTINE_COROUTINE_SCHEDULER_H

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include "coroutine.h"
#include "co_methods.h"
#include "../threadpool/thread_pool.h"

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
			std::shared_ptr<Coroutine> StartCoroutine(std::function<void(CoMethods& coMethods)> fiber);
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

#endif // !__DAXIA_COROUTINE_COROUTINE_SCHEDULER_H
#endif // !_WIN32
