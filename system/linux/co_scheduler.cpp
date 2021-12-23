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

			void CoScheduler::SendTask(std::function<void()>&& fun)
			{
				std::condition_variable notify;
				{
					std::lock_guard<std::mutex> locker(couroutinesLocker_);
					task_.push(Task(std::move(fun), &notify));
					coroutinesNotify_.notify_one();
				}

				std::unique_lock<std::mutex> locker(taskDoneNotifyLocker_);
				notify.wait(locker);
			}

			void CoScheduler::PostTask(std::function<void()>&& fun)
			{
				std::lock_guard<std::mutex> locker(couroutinesLocker_);
				task_.push(Task(std::forward<std::function<void()>>(fun)));
				coroutinesNotify_.notify_one();
			}

			void CoScheduler::Join()
			{
				while (true)
				{
					while (!couroutinesLocker_.try_lock())
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}

					if (coroutines_.empty())
					{
						couroutinesLocker_.unlock();
						break;
					}
					else
					{
						couroutinesLocker_.unlock();
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
			}

			void CoScheduler::Stop()
			{
				run_ = false;

				// 等待停止所有协程
				threadPool_.Stop();
			}

			void CoScheduler::run()
			{
				while (run_)
				{
					daxia::system::DateTime now = daxia::system::DateTime::Now();

					// 调度Task及协程
					Task task;
					std::shared_ptr<Coroutine> work;
					{
						std::unique_lock<std::mutex> locker(couroutinesLocker_);
						if (task_.empty() && coroutines_.empty())
						{
							coroutinesNotify_.wait_for(locker, std::chrono::milliseconds(1000));
						}

						if (!task_.empty())
						{
							task = std::move(task_.front());
							task_.pop();
						}

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
					}

					if (task.fun)
					{
						task.fun();
						if (task.notify)
						{
							std::lock_guard<std::mutex> locker(taskDoneNotifyLocker_);
							task.notify->notify_one();
						}
					}

					if (work)
					{
						// 设置当前协程
						pthread_setspecific(currCoroutineKey_, work.get());

						// 切换协程
						++work->wakeupCount_;
						swapcontext(&mainCtx_, &(work->ctx_));
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
