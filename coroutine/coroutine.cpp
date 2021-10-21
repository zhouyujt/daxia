#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "coroutine.h"

namespace daxia
{
	namespace coroutine
	{
		Coroutine::Coroutine(std::function<void(CoMethods& coMethods)> fiber, long long id, jmp_buf& root)
			: fiber_(fiber)
			, id_(id)
			, wakeupCount_(0)
			, methods_(root,context_)
		{

		}

		Coroutine::~Coroutine()
		{
			
		}
	}
}