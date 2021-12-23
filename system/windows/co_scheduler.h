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
				// ����һ��Э��
				std::shared_ptr<Coroutine> StartCoroutine(std::function<void()>&& fiber, size_t stackSize = 1024 * 1024/*����Э��ջ��С��Ĭ��1M��ָ������ֵ������ȡ��Ϊ4KB�ı���*/);
				// �õ���������ִ��һ�����񣬲��ȴ�ִ�����
				void SendTask(std::function<void()>&& fun);
				// �õ���������ִ��һ�����񣬲��ȴ�ִ����Ͼͷ���
				void PostTask(std::function<void()>&& fun);
				// �ȴ�����Э���������
				void Join();
				// ֹͣ����Э��
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
