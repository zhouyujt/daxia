#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "coroutine.h"

namespace daxia
{
	namespace system
	{
		namespace windows
		{
			Coroutine::Coroutine(std::function<void()>&& fiber, size_t stackSize, long long id, void** mainFiber)
				: id_(id)
				, wakeupCount_(0)
				, complete_(false)
				, mainFiber_(mainFiber)
				, terminate_(false)
				, sleepMilliseconds_(0)
				, yield_(false)
				, wakeupCondition_(nullptr)
			{
				completeEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);

				fiberStartRoutine_ = [&, fiber]()
				{
					// 调用回调
					fiber();

					// 完成标志
					complete_ = true;
					::SetEvent(completeEvent_);

					// 返回主协程
					::SwitchToFiber(*mainFiber_);
				};

				// statckSize取整
				const size_t defaultSize = static_cast<size_t>(1024) * 4;
				stackSize = stackSize == 0 ? defaultSize : (stackSize + (defaultSize - 1)) / defaultSize * defaultSize;

				// 设置本协程入口点
				fiber_ = ::CreateFiber(8193, &Coroutine::fiberStartRoutine, this);
			}

			Coroutine::~Coroutine()
			{
				::CloseHandle(completeEvent_);
				::DeleteFiber(fiber_);
			}

			void Coroutine::Join()
			{
				::WaitForSingleObject(completeEvent_, INFINITE);
			}

			void Coroutine::Terminate()
			{
				// 设置结束标志
				terminate_ = true;

				// 等待结束
				Join();
			}

			void WINAPI Coroutine::fiberStartRoutine(LPVOID param)
			{
				Coroutine* instance = reinterpret_cast<Coroutine*>(param);
				instance->fiberStartRoutine_();
			}
		}
	}
}

#endif