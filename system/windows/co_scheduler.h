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
#include <queue>
#include "coroutine.h"
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
				// 创建一个协程
				std::shared_ptr<Coroutine> StartCoroutine(std::function<void()>&& fiber, size_t stackSize = 1024 * 1024/*设置协程栈大小，默认1M。指定的数值将向上取整为4KB的倍数*/);
				// 让调度器立即执行一个任务，并等待执行完毕
				void SendTask(std::function<void()>&& fun);
				// 让调度器立即执行一个任务，不等待执行完毕就返回
				void PostTask(std::function<void()>&& fun);
				// 等待所有协程运行完毕
				void Join();
				// 停止所有协程
				void Stop();
			private:
				void run();
				void addCoroutine(std::shared_ptr<Coroutine> coroutine);
				long long makeCoroutineId();
			private:
				struct Task
				{
					std::function<void()> fun;
					std::condition_variable* notify;
					Task() : notify(nullptr) {}
					Task(std::function<void()>&& f, std::condition_variable* n = nullptr) 
						: fun(std::forward<std::function<void()>>(f))
						, notify(n) {}
					Task(Task&& task)
					{
						fun = std::move(task.fun);
						notify = task.notify;
					}

					Task& operator=(Task&& task)
					{
						fun = std::move(task.fun);
						notify = task.notify;

						return *this;
					}
				};
			private:
				daxia::system::ThreadPool threadPool_;
				std::list<std::shared_ptr<Coroutine>> coroutines_;
				std::mutex couroutinesLocker_;
				std::condition_variable coroutinesNotify_;
				std::mutex idLocker_;
				std::queue<Task> task_;
				std::mutex taskDoneNotifyLocker_;
				bool run_;
				static long long nextId_;
				void* mainFiber_;
			};
		}
	}
}

#endif // !__DAXIA_SYSTEM_WINDOWS_COSCHEDULER_H
#endif // !_WIN32
