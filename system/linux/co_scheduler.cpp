#ifdef __linux__
#include "co_scheduler.h"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			long long CoScheduler::nextId_ = 0;
			pthread_key_t CoScheduler::currCoroutineKey_;
			std::once_flag CoScheduler::onceCreateKeyFlag_;

			CoScheduler::CoScheduler()
				: threadPool_(1)
				, run_(true)
			{
				threadPool_.Post(std::bind(&CoScheduler::run, this));

				// 创建线程局部变量，保存当前运行的协程，以便在this_coroutine中可以正确识别当前的协程
				std::call_once(onceCreateKeyFlag_, [&]()
					{
						pthread_key_create(&currCoroutineKey_, nullptr);
					});
			}

			CoScheduler::~CoScheduler()
			{
				Stop();
			}

			std::shared_ptr<Coroutine> CoScheduler::StartCoroutine(std::function<void()> fiber, size_t stackSize)
			{
				std::shared_ptr<Coroutine> co(new Coroutine(fiber, stackSize, makeCoroutineId(), &mainCtx_));
				addCoroutine(co);

				return co;
			}

			void CoScheduler::Stop()
			{
				run_ = false;

				// 等待停止所有协程
				threadPool_.Stop();
			}

			void CoScheduler::run()
			{
				// 当没有协程需要调度时的睡眠时间（单位:毫秒）
				const long idle = 1;

				while (run_)
				{
					// 调度协程
					// 调度规则：
					// 新建立的协程立即执行
					// 协程主动让出执行权
					daxia::system::DateTime now = daxia::system::DateTime::Now();
					std::shared_ptr<Coroutine> work;
					{
						couroutinesLocker_.lock();
						for (auto iter = coroutines_.begin(); iter != coroutines_.end();)
						{
							Coroutine& co = *(*iter);

							// 移除完成的协程
							if (co.complete_)
							{
								iter = coroutines_.erase(iter);
								continue;
							}

							// 强制结束协程
							if (co.terminate_)
							{
								sem_post(&co.completeEvent_);
								iter = coroutines_.erase(iter);
								continue;
							}

							// 协程是否睡眠中
							if (co.sleepMilliseconds_ > 0)
							{
								if ((now - co.sleepTimestamp_).Milliseconds() >= co.sleepMilliseconds_)
								{
									work = *iter;

									co.sleepMilliseconds_ = 0;

									break;
								}
								else
								{
									++iter;
									continue;
								}
							}

							// 协程是否挂起中
							if (co.yield_)
							{
								co.yield_ = false;
								++iter;
								continue;
							}

							// 协程是否等待中
							if (co.wakeupCondition_)
							{
								if (co.wakeupCondition_())
								{
									work = *iter;

									// 清空等待状态
									std::function<bool()> empty;
									co.wakeupCondition_.swap(empty);

									break;
								}
								else
								{
									++iter;
									continue;
								}
							}

							work = *iter;
							break;
						}
						couroutinesLocker_.unlock();
					}

					if (work)
					{
						// 设置当前协程
						pthread_setspecific(currCoroutineKey_, work.get());

						// 切换协程
						++work->wakeupCount_;
						swapcontext(&mainCtx_, &(work->ctx_));
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(idle));
					}
				}
			}

			void CoScheduler::addCoroutine(std::shared_ptr<Coroutine> coroutine)
			{
				std::lock_guard<std::mutex> locker(couroutinesLocker_);
				coroutines_.push_back(coroutine);
			}

			long long CoScheduler::makeCoroutineId()
			{
				std::lock_guard<std::mutex> locker(idLocker_);

				++nextId_;

				return nextId_;
			}

		}
	}
}
#endif
