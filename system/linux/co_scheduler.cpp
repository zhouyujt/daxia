#ifdef __linux__
#include "co_scheduler.h"
namespace daxia
{
	namespace system
	{
		namespace linux
		{
			long long CoScheduler::nextId_ = 0;

			CoScheduler::CoScheduler()
				: threadPool_(false)
				, run_(true)
			{
				threadPool_.Post(std::bind(&CoScheduler::run, this));
			}

			CoScheduler::~CoScheduler()
			{
				run_ = false;
			}

			std::shared_ptr<Coroutine> CoScheduler::StartCoroutine(std::function<void(CoMethods& coMethods)> fiber)
			{
				std::shared_ptr<Coroutine> co(new Coroutine(fiber, makeCoroutineId(), &mainCtx_));
				addCoroutine(co);

				return co;
			}

			void CoScheduler::run()
			{
				while (run_)
				{
					// ��û��Э����Ҫ����ʱ��˯��ʱ�䣨��λ:���룩
					const long idle = 10;

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
								sem_post(&co.completeEvent_);
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
								if (co.wakeupCondition_->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
								{
									work = *iter;

									// ��յȴ�״̬
									co.wakeupCondition_ = nullptr;

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
				coroutines_.push_front(coroutine);
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
