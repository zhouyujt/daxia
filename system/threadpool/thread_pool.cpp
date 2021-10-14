#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "thread_pool.h"

namespace daxia
{
	namespace system
	{

		ThreadPool::ThreadPool(bool autoStart /*= true*/)
		{
			if (autoStart)
			{
				Start(GetCpuCoreCount() * 2);
			}
		}

		ThreadPool::~ThreadPool()
		{
			Stop();
		}

		void ThreadPool::Start(size_t count)
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

		void ThreadPool::Stop()
		{
			ios_.stop();
			for (size_t i = 0; i < threads_.size(); ++i)
			{
				if (threads_[i].joinable())
				{
					threads_[i].join();
				}
			}
			ios_.reset();
			threads_.clear();
		}

		void ThreadPool::Dispatch(std::function<void()> work)
		{
			ios_.post(work);
		}

		size_t ThreadPool::GetCpuCoreCount()
		{
			size_t count = std::thread::hardware_concurrency();

			return count == 0 ? 1 : count;
		}

	}
}