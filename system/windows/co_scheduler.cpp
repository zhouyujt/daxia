#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "co_scheduler.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			long long CoScheduler::nextId_ = 0;

			CoScheduler::CoScheduler()
				: threadPool_(1)
				, run_(true)
				, mainFiber_(nullptr)
			{
				threadPool_.Post(std::bind(&CoScheduler::run, this));
			}

			CoScheduler::~CoScheduler()
			{
				Stop();
			}

			std::shared_ptr<Coroutine> CoScheduler::StartCoroutine(std::function<void()>&& fiber, size_t stackSize)
			{
				std::shared_ptr<Coroutine> co(new Coroutine(std::forward<std::function<void()>>(fiber), stackSize, makeCoroutineId(), &mainFiber_));
				addCoroutine(co);

				return co;
			}

			void CoScheduler::Join()
			{
				while (true)
				{
					while (!couroutinesLocker_.try_lock())
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}

					if (coroutines_.empty())
					{
						couroutinesLocker_.unlock();
						break;
					}
					else
					{
						couroutinesLocker_.unlock();
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
			}

			void CoScheduler::Stop()
			{
				run_ = false;

				// 等待停止所有协程
				threadPool_.Join();
			}

			void CoScheduler::run()
			{
				// 当没有协程需要调度时的睡眠时间（单位:毫秒）
				const long idle = 1;

				// 设置主协程入口
				mainFiber_ = ConvertThreadToFiber(nullptr);

				while (run_)
				{
					// 调度协程
					// 调度规则：
					// 新建立的协程立即执行
					// 协程主动让出执行权
					daxia::system::DateTime now = daxia::system::DateTime::Now();
					std::shared_ptr<Coroutine> work;
					{
						couroutinesLocker_.lock();
						for (auto iter = coroutines_.begin(); iter != coroutines_.end();)
						{
							Coroutine& co = *(*iter);

							// 移除完成的协程
							if (co.complete_)
							{
								iter = coroutines_.erase(iter);
								continue;
							}

							// 强制结束协程
							if (co.terminate_)
							{
								::SetEvent(co.completeEvent_);
								iter = coroutines_.erase(iter);
								continue;
							}

							// 协程是否睡眠中
							if (co.sleepMilliseconds_ > 0)
							{
								if ((now - co.sleepTimestamp_).Milliseconds() >= co.sleepMilliseconds_)
								{
									work = *iter;

									co.sleepMilliseconds_ = 0;

									break;
								}
								else
								{
									++iter;
									continue;
								}
							}

							// 协程是否挂起中
							if (co.yield_)
							{
								co.yield_ = false;
								++iter;
								continue;
							}

							// 协程是否等待中
							if (co.wakeupCondition_)
							{
								if (co.wakeupCondition_())
								{
									work = *iter;

									// 清空等待状态
									std::function<bool()> empty;
									co.wakeupCondition_.swap(empty);

									break;
								}
								else
								{
									++iter;
									continue;
								}
							}

							work = *iter;
							break;
						}
						couroutinesLocker_.unlock();
					}

					if (work)
					{
						++work->wakeupCount_;
						::SwitchToFiber(work->fiber_);
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(idle));
					}
				}
			}

			void CoScheduler::addCoroutine(std::shared_ptr<Coroutine> coroutine)
			{
				std::lock_guard<std::mutex> locker(couroutinesLocker_);
				coroutines_.push_back(coroutine);
			}

			long long CoScheduler::makeCoroutineId()
			{
				std::lock_guard<std::mutex> locker(idLocker_);

				++nextId_;

				return nextId_;
			}

		}
	}
}
#endif
