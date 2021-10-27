#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "this_coroutine.h"
#include "coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			namespace this_coroutine
			{
				Coroutine* getCurrentCoroutine()
				{
					return  reinterpret_cast<Coroutine*>(::GetFiberData());
				}

				void CoSleep(size_t milliseconds)
				{
					Coroutine* co = getCurrentCoroutine();

					co->sleepTimestamp_ = daxia::system::DateTime::Now();
					co->sleepMilliseconds_ = milliseconds;
					::SwitchToFiber(*co->mainFiber_);
				}

				void CoYield()
				{
					Coroutine* co = getCurrentCoroutine();

					co->yield_ = true;
					::SwitchToFiber(*co->mainFiber_);
				}

				void CoWait(std::function<bool()>&& wakeupCondition)
				{
					Coroutine* co = getCurrentCoroutine();

					co->wakeupCondition_.swap(wakeupCondition);
					::SwitchToFiber(*co->mainFiber_);
				}
			}
		}
	}
}
#endif // !_WIN32