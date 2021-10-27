/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file thread_pool.h
* \author 漓江里的大虾
* \date 八月 2021
*
* 通用线程池
*
*/
#ifndef __DAXIA_SYSTEM_THREADPOOL_H
#define __DAXIA_SYSTEM_THREADPOOL_H
#include <functional>
#include <thread>
#include <vector>
#include <future>
#include <boost/asio.hpp>

namespace daxia
{
	namespace system
	{
		class ThreadPool
		{
		public:
			ThreadPool(size_t threadCount = 0 // 线程数量。如为0则为cpu核数*2
				);
			~ThreadPool();
		public:
			// 分发一个任务,该任务加入任务列表，线程池将自动寻找一个空闲的线程执行该任务
			template<typename T>
			void Post(std::packaged_task<T()>& task)
			{
				ios_.post(std::ref(task));
			}

			void Post(std::function<void()>&& fun)
			{
				ios_.post(std::forward<std::function<void()>>(fun));
			}

			// 分发一个任务，如果调用此方法的线程为线程池中的线程则立即执行，否则同Post。
			template<typename T>
			void Dispatch(std::packaged_task<T()>& task)
			{
				ios_.dispatch(std::ref(task));
			}

			void Dispatch(std::function<void()>&& fun)
			{
				ios_.dispatch(std::forward<std::function<void()>>(fun));
			}

			// 停止所有线程
			void Stop();
		public:
			// 获取CPU核心数量
			static size_t GetCpuCoreCount();
		private:
			std::vector<std::thread> threads_;
			boost::asio::io_service ios_;
		private:
			void start(size_t count);
		};

	}
}

#endif // !__DAXIA_SYSTEM_THREADPOOL_H