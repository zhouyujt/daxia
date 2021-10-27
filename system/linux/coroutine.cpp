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
					// 调用回调
					fiber();

					// 完成标志
					complete_ = true;
					sem_post(&completeEvent_);

					// 返回主协程
					swapcontext(&ctx_, mainCtx_);
				};

				// 设置本协程入口点
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
				// 设置结束标志
				terminate_ = true;

				// 等待结束
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