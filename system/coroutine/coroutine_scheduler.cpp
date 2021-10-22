#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "coroutine_scheduler.h"
namespace daxia
{
	namespace coroutine
	{
		long long Scheduler::nextId_ = 0;

		Scheduler::Scheduler()
			: threadPool_(false)
			, run_(true)
			, mainFiber_(nullptr)
		{
			threadPool_.Start(1);
			threadPool_.Dispatch(std::bind(&Scheduler::run,this));
		}

		Scheduler::~Scheduler()
		{
			run_ = false;
			threadPool_.Stop();
		}

		std::shared_ptr<daxia::coroutine::Coroutine> Scheduler::StartCoroutine(std::function<void(CoMethods& coMethods)> fiber)
		{
			// 等待Scheduler::run启动完成
			for (size_t i = 0; i < 10; ++i)
			{
				if (mainFiber_ == nullptr)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				else
				{
					break;
				}
			}

			// 启动失败
			if (mainFiber_ == nullptr) return nullptr;

			std::shared_ptr<daxia::coroutine::Coroutine> co(new Coroutine(fiber, makeCoroutineId(), mainFiber_));
			addCoroutine(co);

			return co;
		}

		void Scheduler::run()
		{
			mainFiber_ = ConvertThreadToFiber(nullptr);

			while (run_)
			{
				// 当没有协程需要调度时的睡眠时间（单位:毫秒）
				const long idle = 10;

				// 调度协程
				// 调度规则：
				// 新建立的协程立即执行
				// 协程主动让出执行权
				daxia::system::DateTime now = daxia::system::DateTime::Now();
				std::shared_ptr<daxia::coroutine::Coroutine> work;
				{
					couroutinesLocker_.lock();
					for (auto iter = coroutines_.begin(); iter != coroutines_.end();)
					{
						daxia::coroutine::Coroutine& co = *(*iter);

						// 移除完成的协程
						if (co.complete_)
						{
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
								co.wakeupCondition_ = std::function<bool()>();

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

		void Scheduler::addCoroutine(std::shared_ptr<Coroutine> coroutine)
		{
			std::lock_guard<std::mutex> locker(couroutinesLocker_);
			coroutines_.push_front(coroutine);
		}

		void Scheduler::delCoroutine(std::shared_ptr<Coroutine> coroutine)
		{
			std::lock_guard<std::mutex> locker(couroutinesLocker_);
			for (auto iter = coroutines_.begin(); iter != coroutines_.end(); ++iter)
			{
				if ((*iter)->Id() == coroutine->Id())
				{
					coroutines_.erase(iter);
					break;
				}
			}
		}

		long long Scheduler::makeCoroutineId()
		{
			std::lock_guard<std::mutex> locker(idLocker_);

			++nextId_;

			return nextId_;
		}

	}
}
#endif
