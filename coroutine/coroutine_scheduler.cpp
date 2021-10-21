#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "coroutine_scheduler.h"
#include <iostream>
namespace daxia
{
	namespace coroutine
	{
		long long Scheduler::nextId_ = 0;

		Scheduler::Scheduler()
			: threadPool_(false)
			, run_(true)
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
			std::shared_ptr<daxia::coroutine::Coroutine> co(new Coroutine(fiber, makeCoroutineId(), context_));
			addCoroutine(co);

			return co;
		}

		void Scheduler::run()
		{
			setjmp(context_);
			while (run_)
			{
				// 寻找需调度的协程
				// 调度规则：
				// 1.新建的协程立即执行
				// 2.优先调度挂起时间最久的协程
				std::shared_ptr<daxia::coroutine::Coroutine> co;
				{
					std::lock_guard<std::mutex> locker(couroutinesLocker_);
					for (auto iter = coroutines_.begin(); iter != coroutines_.end(); ++iter)
					{
						// 新建的协程立即执行
						if ((*iter)->wakeupCount_ == 0)
						{
							co = *iter;
							break;
						}

						if (co)
						{
							if ((*iter)->yieldTimestamp_ > co->yieldTimestamp_)
							{
								co = *iter;
							}
						}
						else
						{
							co = *iter;
						}
					}
				}

				if (co)
				{
					if (co->wakeupCount_ == 0)
					{
						++co->wakeupCount_;
						co->fiber_(co->methods_);
					}
					else
					{
						longjmp(co->context_, 2);
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
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