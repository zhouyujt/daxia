#ifdef __linux__
#include "coroutine.h"
#include <iostream>
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			Coroutine::Coroutine(std::function<void()> fiber, size_t stackSize, long long id, ucontext_t* mainFiber)
				: id_(id)
				, wakeupCount_(0)
				, complete_(false)
				, mainCtx_(mainFiber)
				, terminate_(false)
				, sleepMilliseconds_(0)
				, yield_(false)
				, wakeupCondition_(nullptr)
			{
				sem_init(&completeEvent_, 0, 0);

				fiberStartRoutine_ = [&, fiber, mainFiber]()
				{
					// ���ûص�
					fiber();

					// ��ɱ�־
					complete_ = true;
					sem_post(&completeEvent_);

					// ������Э��
					swapcontext(&ctx_, mainCtx_);
				};

				// statckSizeȡ��
				const size_t defaultSize = static_cast<size_t>(1024) * 4;
				stackSize = stackSize == 0 ? defaultSize : (stackSize + (defaultSize - 1)) / defaultSize * defaultSize;

				// ���ñ�Э����ڵ�
				getcontext(&ctx_);
				stack_ = std::shared_ptr<char>(new char[stackSize]);
				ctx_.uc_stack.ss_sp = stack_.get();
				ctx_.uc_stack.ss_size = stackSize;
				ctx_.uc_stack.ss_flags = 0;
				makecontext(&ctx_, (void(*)())(fiberStartRoutine), 1, this);
			}

			Coroutine::~Coroutine()
			{
				sem_destroy(&completeEvent_);
			}

			void Coroutine::Join()
			{
				sem_wait(&completeEvent_);
			}

			void Coroutine::Terminate()
			{
				// ���ý�����־
				terminate_ = true;

				// �ȴ�����
				Join();
			}

			void Coroutine::fiberStartRoutine(Coroutine* co)
			{
				co->fiberStartRoutine_();
			}
		}
	}
}

#endif