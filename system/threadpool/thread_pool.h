/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file datetime.h
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
#include <boost/asio.hpp>

namespace daxia
{
	namespace system
	{
		class ThreadPool
		{
		public:
			ThreadPool(bool autoStart = true);
			~ThreadPool();
		public:
			// 启动指定数量的线程，并等待Dispathch的工作任务
			// count: 线程数量。推荐系统cpu核数 * ２
			void Start(size_t count);

			// 停止所有的线程
			void Stop();

			// 分发一个任务。线程池将自动寻找一个空闲的线程执行该任务
			// work: 需要执行的任务
			void Dispathch(std::function<void()> work);
		public:
			// 获取CPU核心数量
			static size_t GetCpuCoreCount();
		private:
			std::vector<std::thread> threads_;
			boost::asio::io_service ios_;
		};
	}
}

#endif // !__DAXIA_SYSTEM_THREADPOOL_H