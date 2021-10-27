#ifdef __linux__
#include "coroutine.h"

#define CO_STACK_SIZE 1024 * 8
#include <iostream>
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			Coroutine::Coroutine(std::function<void()> fiber, long long id, ucontext_t* mainFiber)
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

				// ���ñ�Э����ڵ�
				getcontext(&ctx_);
				stack_ = std::shared_ptr<char>(new char[CO_STACK_SIZE]);
				ctx_.uc_stack.ss_sp = stack_.get();
				ctx_.uc_stack.ss_size = CO_STACK_SIZE;
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