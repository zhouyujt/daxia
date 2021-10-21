#include "co_methods.h"

namespace daxia
{
	namespace coroutine
	{

		CoMethods::CoMethods(jmp_buf& root, jmp_buf& context)
			: root_(root)
			, context_(context)
		{

		}

		CoMethods::~CoMethods()
		{

		}

		void CoMethods::CoSleep(size_t milliseconds)
		{

		}

		void CoMethods::CoYield(std::function<void()> wakeupCondition)
		{
			if (setjmp(context_) == 0)
			{
				longjmp(root_, 1);
			}
			else
			{
				return;
			}
		}

	}
}