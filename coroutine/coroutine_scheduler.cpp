#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "coroutine_scheduler.h"

namespace daxia
{
	namespace coroutine
	{
		Scheduler::Scheduler()
			: threadPool_(false)
		{
			threadPool_.Start(1);
			//threadPool_.Dispatch(std::bind(Scheduler::run,this));
		}

		Scheduler::~Scheduler()
		{

		}

		std::shared_ptr<daxia::coroutine::Coroutine> Scheduler::StartCoroutine(std::function<void()> fiber)
		{
			addCoroutine(fiber);
			throw 1;
		}

		void Scheduler::run()
		{

		}

		void Scheduler::addCoroutine(std::function<void()> fiber)
		{
			std::lock_guard<std::mutex> locker(couroutinesLocker_);
			//fibers_.push_back(fiber);
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
	}
}