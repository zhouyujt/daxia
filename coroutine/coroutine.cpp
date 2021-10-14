#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "coroutine.h"

namespace daxia
{
	namespace coroutine
	{
		Coroutine::Coroutine(std::function<void()> fiber, long long id)
			: fiber_(fiber)
			, id_(id)
		{

		}

		Coroutine::~Coroutine()
		{
			
		}
	}
}