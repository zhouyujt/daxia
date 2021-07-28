#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "scheduler.h"
#include "session.h"

namespace daxia
{
	namespace net
	{
		Scheduler::Scheduler()
			: fps_(20)
			, isWorking_(false)
		{
		}

		Scheduler::~Scheduler()
		{
			Stop();
		}

		void Scheduler::SetFPS(unsigned long fps)
		{
			fps_ = fps;
		}

		long long Scheduler::ScheduleUpdate(scheduleFunc func)
		{
			lock_guard locker(scheduleLocker_);

			UpdateFunc updateFunc;
			updateFunc.id = makeScheduleID();
			updateFunc.f = func;

			updateFuncs_.push_back(updateFunc);

			return updateFunc.id;
		}

		long long Scheduler::Schedule(scheduleFunc func, unsigned long duration)
		{
			using namespace std::chrono;

			lock_guard locker(scheduleLocker_);

			ScheduleFunc scheduleFunc;
			scheduleFunc.id = makeScheduleID();
			scheduleFunc.f = func;
			scheduleFunc.duration = duration;
			scheduleFunc.isOnce = false;
			scheduleFunc.timestamp = time_point_cast<milliseconds>(system_clock::now());

			scheduleFuncs_.push_back(scheduleFunc);

			return scheduleFunc.id;
		}

		long long Scheduler::ScheduleOnce(scheduleFunc func, unsigned long duration)
		{
			using namespace std::chrono;
			lock_guard locker(scheduleLocker_);

			ScheduleFunc scheduleFunc;
			scheduleFunc.id = makeScheduleID();
			scheduleFunc.f = func;
			scheduleFunc.duration = duration;
			scheduleFunc.isOnce = true;
			scheduleFunc.timestamp = time_point_cast<milliseconds>(system_clock::now());

			scheduleFuncs_.push_back(scheduleFunc);

			return scheduleFunc.id;
		}

		void Scheduler::UnscheduleUpdate(long long scheduleID)
		{
			lock_guard locker(scheduleLocker_);

			for (auto iter = updateFuncs_.begin(); iter != updateFuncs_.end(); ++iter)
			{
				if (iter->id == scheduleID)
				{
					updateFuncs_.erase(iter);
					break;
				}
			}
		}

		void Scheduler::Unschedule(long long scheduleID)
		{
			lock_guard locker(scheduleLocker_);

			for (auto iter = scheduleFuncs_.begin(); iter != scheduleFuncs_.end(); ++iter)
			{
				if (iter->id == scheduleID)
				{
					scheduleFuncs_.erase(iter);
					break;
				}
			}
		}

		void Scheduler::UnscheduleAll()
		{
			lock_guard locker(scheduleLocker_);

			updateFuncs_.clear();
			scheduleFuncs_.clear();
		}

		void Scheduler::SetNetDispatch(netDispatchFunc func)
		{
			dispatch_ = func;
		}

		void Scheduler::PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback)
		{
			lock_guard locker(netRequestLocker_);
			netRequests_.push(NetRequest(session, msgId, data, finishCallback));
		}

		void Scheduler::Run()
		{
			isWorking_ = true;

			workThread_ = std::thread([&]()
			{
				const unsigned long interval = 1000 / fps_;

				while (isWorking_)
				{
					using namespace std::chrono;

					// begin time
					time_point<system_clock, milliseconds> beginTime = time_point_cast<milliseconds>(system_clock::now());

					// 更新调度器
					for (auto iter = updateFuncs_.begin(); iter != updateFuncs_.end(); ++iter)
					{
						iter->f();
					}

					// 定时调度器
					for (auto iter = scheduleFuncs_.begin(); iter != scheduleFuncs_.end();)
					{
						if ((beginTime - iter->timestamp).count() >= iter->duration)
						{
							iter->f();
							iter->timestamp = beginTime;

							if (iter->isOnce)
							{
								iter = scheduleFuncs_.erase(iter);
								continue;
							}
						}

						++iter;
					}

					// 获取网络请求
					for (;;)
					{
						NetRequest r;
						netRequestLocker_.lock();
						if (!netRequests_.empty())
						{
							r = netRequests_.front();
							netRequests_.pop();
						}
						netRequestLocker_.unlock();

						// 处理网络请求
						if (r.session != nullptr)
						{
							if (dispatch_)
							{
								dispatch_(r.session, r.msgId, r.data);
							}

							if (r.finishCallback)
							{
								r.finishCallback();
							}
						}
						else
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}

						// stop time
						time_point<system_clock, milliseconds> stopTime = time_point_cast<milliseconds>(system_clock::now());

						if ((stopTime - beginTime).count() >= interval)
						{
							break;
						}
					}
				}
			});
		}

		void Scheduler::Stop()
		{
			isWorking_ = false;

			if (workThread_.joinable())
			{
				workThread_.join();
			}
		}

		long long Scheduler::makeScheduleID()
		{
			lock_guard locker(nextScheduleIDLocker_);

			return nextScheduleID_++;
		}
	}// namespace net
}// namespace daxia

