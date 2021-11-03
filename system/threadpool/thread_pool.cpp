#include "thread_pool.h"

namespace daxia
{
	namespace system
	{

		ThreadPool::ThreadPool(size_t threadCount)
		{
			threadCount = threadCount == 0 ? GetCpuCoreCount() * 2 : threadCount;
			start(threadCount);	
		}

		ThreadPool::~ThreadPool()
		{
			Stop();
		}

		void ThreadPool::Join()
		{
			for (size_t i = 0; i < threads_.size(); ++i)
			{
				if (threads_[i].joinable())
				{
					threads_[i].join();
				}
			}
		}

		void ThreadPool::Stop()
		{
			ios_.stop();
			Join();
			ios_.reset();
			threads_.clear();
		}

		size_t ThreadPool::GetCpuCoreCount()
		{
			size_t count = std::thread::hardware_concurrency();

			return count == 0 ? 1 : count;
		}

		void ThreadPool::start(size_t count)
		{
			if (!threads_.empty()) return;

			for (size_t i = 0; i < count; ++i)
			{
				threads_.push_back(
					std::thread([=]()
				{
					boost::asio::io_service::work worker(ios_);
					ios_.run();
				}));
			}
		}
	}
}