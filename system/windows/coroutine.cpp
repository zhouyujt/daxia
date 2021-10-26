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
			Coroutine::Coroutine(std::function<void(CoMethods& coMethods)>&& fiber, long long id, void* mainFiber)
				: id_(id)
				, wakeupCount_(0)
				, complete_(false)
				, methods_(this)
				, mainFiber_(mainFiber)
				, terminate_(false)
				, sleepMilliseconds_(0)
				, yield_(false)
				, wakeupCondition_(nullptr)
			{
				completeEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);

				fiberStartRoutine_ = [&, fiber, mainFiber]()
				{
					// ���ûص�
					fiber(methods_);

					// ��ɱ�־
					complete_ = true;
					::SetEvent(completeEvent_);

					// ������Э��
					::SwitchToFiber(mainFiber);
				};

				fiber_ = ::CreateFiber(0, &Coroutine::fiberStartRoutine, this);
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
				// ���ý�����־
				terminate_ = true;

				// �ȴ�����
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