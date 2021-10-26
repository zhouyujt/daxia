#ifdef __linux__
#include "co_methods.h"
#include "coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			CoMethods::CoMethods(Coroutine* coroutine)
				: couroutine_(coroutine)
			{

			}

			CoMethods::~CoMethods()
			{

			}

			void CoMethods::CoSleep(size_t milliseconds)
			{
				couroutine_->sleepTimestamp_ = daxia::system::DateTime::Now();
				couroutine_->sleepMilliseconds_ = milliseconds;
				swapcontext(&couroutine_->ctx_, couroutine_->mainCtx_);
			}

			void CoMethods::CoYield()
			{
				couroutine_->yield_ = true;
				swapcontext(&couroutine_->ctx_, couroutine_->mainCtx_);
			}

			void CoMethods::CoWait(std::function<bool()>&& wakeupCondition)
			{
				couroutine_->wakeupCondition_.swap(wakeupCondition);
				swapcontext(&couroutine_->ctx_, couroutine_->mainCtx_);
			}
		}
	}
}
#endif // !__linux__