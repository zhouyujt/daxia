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
			, enableFps_(false)
			, isWorking_(false)
			, nextScheduleID_(0)
		{
		}

		Scheduler::~Scheduler()
		{
			Stop();
		}

		void Scheduler::SetFps(unsigned long fps)
		{
			if (fps == 0) fps = 1;

			fps_ = fps;
		}

		long long Scheduler::ScheduleUpdate(scheduleFunc func)
		{
			if (enableFps_)
			{
				lock_guard locker(scheduleLocker_);

				UpdateFunc updateFunc;
				updateFunc.id = makeScheduleID();
				updateFunc.f = func;

				updateFuncs_.push_back(updateFunc);

				return updateFunc.id;
			}
			else
			{
				return -1;
			}
		}

		long long Scheduler::Schedule(scheduleFunc func, unsigned long duration)
		{
			using namespace std::chrono;

			if (enableFps_)
			{
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
			else
			{
				lock_guard locker(scheduleLocker_);

				long long id = makeScheduleID();

				timers_[id] = new boost::asio::deadline_timer(logicIoService_, boost::posix_time::milliseconds(duration));
				timers_[id]->async_wait(std::bind(&Scheduler::asyncWaitCB, this, func, id, duration, std::placeholders::_1));

				return id;
			}
		}

		long long Scheduler::ScheduleOnce(scheduleFunc func, unsigned long duration)
		{
			using namespace std::chrono;

			if (enableFps_)
			{

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
			else
			{
				lock_guard locker(scheduleLocker_);

				long long id = makeScheduleID();

				timers_[id] = new boost::asio::deadline_timer(logicIoService_, boost::posix_time::milliseconds(duration));
				timers_[id]->async_wait([&, func, id](const boost::system::error_code& ec)
				{
					if (!ec)
					{
						func();
					}

					lock_guard locker(scheduleLocker_);
					std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(id);
					if (iter != timers_.end())
					{
						delete iter->second;
						timers_.erase(iter);
					}
				});

				return id;
			}
		}

		void Scheduler::UnscheduleUpdate(long long scheduleID)
		{
			if (enableFps_)
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
		}

		void Scheduler::Unschedule(long long scheduleID)
		{
			if (enableFps_)
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
			else
			{
				lock_guard locker(scheduleLocker_);

				std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(scheduleID);
				if (iter != timers_.end())
				{
					iter->second->cancel();
					delete iter->second;
					timers_.erase(iter);
				}
			}
		}

		void Scheduler::UnscheduleAll()
		{
			if (enableFps_)
			{
				lock_guard locker(scheduleLocker_);

				updateFuncs_.clear();
				scheduleFuncs_.clear();
			}
			else
			{
				lock_guard locker(scheduleLocker_);

				for (auto iter = timers_.begin(); iter != timers_.end(); ++iter)
				{
					iter->second->cancel();
					delete iter->second;
				}

				timers_.clear();
			}
		}

		void Scheduler::SetNetDispatch(netDispatchFunc func)
		{
			dispatch_ = func;
		}

		void Scheduler::PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer data, std::function<void()> finishCallback)
		{
			lock_guard locker(netRequestLocker_);
			netRequests_.push(NetRequest(session, msgId, data, finishCallback));

			if (!enableFps_)
			{
				logicIoService_.post([&]()
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
				});
			}
		}

		void Scheduler::Run(bool enableFps)
		{
			enableFps_ = enableFps;

			if (enableFps_)
			{
				runAsFps();
			}
			else
			{
				runAsNoFps();
			}
		}

		void Scheduler::Stop()
		{
			if (enableFps_)
			{

				isWorking_ = false;

				if (workThread_.joinable())
				{
					workThread_.join();
				}
			}
			else
			{
				// 停止所有定时器
				lock_guard locker(scheduleLocker_);
				for (auto iter = timers_.begin(); iter != timers_.end(); ++iter)
				{
					iter->second->cancel();
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					delete iter->second;
				}
				timers_.clear();

				// 停止逻辑线程
				logicIoService_.stop();
				for (size_t i = 0; i < logicThreads_.size(); ++i)
				{
					if (logicThreads_[i].joinable())
					{
						logicThreads_[i].join();
					}
				}

				logicThreads_.clear();
			}
		}

		long long Scheduler::makeScheduleID()
		{
			lock_guard locker(nextScheduleIDLocker_);

			return nextScheduleID_++;
		}

		void Scheduler::runAsFps()
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

		void Scheduler::runAsNoFps()
		{
			//int coreCount = getCoreCount();

			//for (int i = 0; i < coreCount * 2; ++i)
			for (int i = 0; i < 1; ++i)
			{
				logicThreads_.push_back(std::thread([&]()
				{
					boost::asio::io_service::work worker(logicIoService_);
					logicIoService_.run();
				}));
			}

		}

		void Scheduler::asyncWaitCB(scheduleFunc func, long long id, long long duration, const boost::system::error_code& ec)
		{
			if (!ec)
			{
				func();

				std::map<long long, boost::asio::deadline_timer*>::iterator iter = timers_.find(id);
				if (iter != timers_.end())
				{
					iter->second->expires_at(iter->second->expires_at() + boost::posix_time::milliseconds(duration));
					iter->second->async_wait(std::bind(&Scheduler::asyncWaitCB, this, func, id, duration, std::placeholders::_1));
				}
			}
		}

	}// namespace net
}// namespace daxia

