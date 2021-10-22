#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "coroutine_scheduler.h"
namespace daxia
{
	namespace coroutine
	{
		long long Scheduler::nextId_ = 0;

		Scheduler::Scheduler()
			: threadPool_(false)
			, run_(true)
			, mainFiber_(nullptr)
		{
			threadPool_.Start(1);
			threadPool_.Dispatch(std::bind(&Scheduler::run,this));
		}

		Scheduler::~Scheduler()
		{
			run_ = false;
			threadPool_.Stop();
		}

		std::shared_ptr<daxia::coroutine::Coroutine> Scheduler::StartCoroutine(std::function<void(CoMethods& coMethods)> fiber)
		{
			// �ȴ�Scheduler::run�������
			for (size_t i = 0; i < 10; ++i)
			{
				if (mainFiber_ == nullptr)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				else
				{
					break;
				}
			}

			// ����ʧ��
			if (mainFiber_ == nullptr) return nullptr;

			std::shared_ptr<daxia::coroutine::Coroutine> co(new Coroutine(fiber, makeCoroutineId(), mainFiber_));
			addCoroutine(co);

			return co;
		}

		void Scheduler::run()
		{
			mainFiber_ = ConvertThreadToFiber(nullptr);

			while (run_)
			{
				// ��û��Э����Ҫ����ʱ��˯��ʱ�䣨��λ:���룩
				const long idle = 10;

				// ����Э��
				// ���ȹ���
				// �½�����Э������ִ��
				// Э�������ó�ִ��Ȩ
				daxia::system::DateTime now = daxia::system::DateTime::Now();
				std::shared_ptr<daxia::coroutine::Coroutine> work;
				{
					couroutinesLocker_.lock();
					for (auto iter = coroutines_.begin(); iter != coroutines_.end();)
					{
						daxia::coroutine::Coroutine& co = *(*iter);

						// �Ƴ���ɵ�Э��
						if (co.complete_)
						{
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
								co.wakeupCondition_ = std::function<bool()>();

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

		void Scheduler::addCoroutine(std::shared_ptr<Coroutine> coroutine)
		{
			std::lock_guard<std::mutex> locker(couroutinesLocker_);
			coroutines_.push_front(coroutine);
		}

		void Scheduler::delCoroutine(std::shared_ptr<Coroutine> coroutine)
		{
			std::lock_guard<std::mutex> locker(couroutinesLocker_);
			for (auto iter = coroutines_.begin(); iter != coroutines_.end(); ++iter)
			{
				if ((*iter)->Id() == coroutine->Id())
				{
					coroutines_.erase(iter);
					break;
				}
			}
		}

		long long Scheduler::makeCoroutineId()
		{
			std::lock_guard<std::mutex> locker(idLocker_);

			++nextId_;

			return nextId_;
		}

	}
}
#endif
