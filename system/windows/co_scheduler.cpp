#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "co_scheduler.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			long long CoScheduler::nextId_ = 0;

			CoScheduler::CoScheduler()
				: threadPool_(1)
				, run_(true)
				, mainFiber_(nullptr)
			{
				threadPool_.Post(std::bind(&CoScheduler::run, this));
			}

			CoScheduler::~CoScheduler()
			{
				Stop();
			}

			std::shared_ptr<Coroutine> CoScheduler::StartCoroutine(std::function<void()>&& fiber, size_t stackSize)
			{
				std::shared_ptr<Coroutine> co(new Coroutine(std::forward<std::function<void()>>(fiber), stackSize, makeCoroutineId(), &mainFiber_));
				addCoroutine(co);

				return co;
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

				// �ȴ�ֹͣ����Э��
				threadPool_.Join();
			}

			void CoScheduler::run()
			{
				// ��û��Э����Ҫ����ʱ��˯��ʱ�䣨��λ:���룩
				const long idle = 1;

				// ������Э�����
				mainFiber_ = ConvertThreadToFiber(nullptr);

				while (run_)
				{
					// ����Э��
					// ���ȹ���
					// �½�����Э������ִ��
					// Э�������ó�ִ��Ȩ
					daxia::system::DateTime now = daxia::system::DateTime::Now();
					std::shared_ptr<Coroutine> work;
					{
						couroutinesLocker_.lock();
						for (auto iter = coroutines_.begin(); iter != coroutines_.end();)
						{
							Coroutine& co = *(*iter);

							// �Ƴ���ɵ�Э��
							if (co.complete_)
							{
								iter = coroutines_.erase(iter);
								continue;
							}

							// ǿ�ƽ���Э��
							if (co.terminate_)
							{
								::SetEvent(co.completeEvent_);
								iter = coroutines_.erase(iter);
								continue;
							}

							// Э���Ƿ�˯����
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

							// Э���Ƿ������
							if (co.yield_)
							{
								co.yield_ = false;
								++iter;
								continue;
							}

							// Э���Ƿ�ȴ���
							if (co.wakeupCondition_)
							{
								if (co.wakeupCondition_())
								{
									work = *iter;

									// ��յȴ�״̬
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
						++work->wakeupCount_;
						::SwitchToFiber(work->fiber_);
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
