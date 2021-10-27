#ifdef __linux__
#include <ucontext.h>
#include <pthread.h>
#include "this_coroutine.h"
#include "coroutine.h"
#include "co_scheduler.h"

namespace daxia
{
	namespace system
	{
		namespace linux
		{
			namespace this_coroutine
			{
				Coroutine* getCurrentCoroutine(pthread_key_t key)
				{
					return reinterpret_cast<Coroutine*>(pthread_getspecific(key));
				}

				void CoSleep(size_t milliseconds)
				{
					Coroutine* co = getCurrentCoroutine(daxia::system::linux::CoScheduler::currCoroutineKey_);

					co->sleepTimestamp_ = daxia::system::DateTime::Now();
					co->sleepMilliseconds_ = milliseconds;
					swapcontext(&co->ctx_, co->mainCtx_);
				}

				void CoYield()
				{
					Coroutine* co = getCurrentCoroutine(daxia::system::linux::CoScheduler::currCoroutineKey_);

					co->yield_ = true;
					swapcontext(&co->ctx_, co->mainCtx_);
				}

				void CoWait(std::function<bool()>&& wakeupCondition)
				{
					Coroutine* co = getCurrentCoroutine(daxia::system::linux::CoScheduler::currCoroutineKey_);

					co->wakeupCondition_.swap(wakeupCondition);
					swapcontext(&co->ctx_, co->mainCtx_);
				}
			}
		}
	}
}
#endif // !__linux__