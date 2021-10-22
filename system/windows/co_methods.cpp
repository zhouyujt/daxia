#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "co_methods.h"
#include "coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace windows
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
				::SwitchToFiber(couroutine_->mainFiber_);
			}

			void CoMethods::CoYield()
			{
				couroutine_->yield_ = true;
				::SwitchToFiber(couroutine_->mainFiber_);
			}

			void CoMethods::CoWait(std::function<bool()> wakeupCondition)
			{
				couroutine_->wakeupCondition_ = wakeupCondition;
				::SwitchToFiber(couroutine_->mainFiber_);
			}
		}
	}
}
#endif // !_WIN32