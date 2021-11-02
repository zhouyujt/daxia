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

				// �����ֲ߳̾����������浱ǰ���е�Э�̣��Ա���this_coroutine�п�����ȷʶ��ǰ��Э��
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

				// �ȴ�ֹͣ����Э��
				threadPool_.Stop();
			}

			void CoScheduler::run()
			{
				// ��û��Э����Ҫ����ʱ��˯��ʱ�䣨��λ:���룩
				const long idle = 1;

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
						// ���õ�ǰЭ��
						pthread_setspecific(currCoroutineKey_, work.get());

						// �л�Э��
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
