#ifdef _WIN32
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

		void Scheduler::PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer& data, std::function<void()> finishCallback)
		{
			lock_guard locker(netRequestLocker_);
			netRequests_.push(NetRequest(session, msgId, data, finishCallback));
			netRequestNotify_.notify_one();
		}

		void Scheduler::Run()
		{
			run();
		}

		void Scheduler::Stop()
		{
			cosc_.Stop();

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

		void Scheduler::run()
		{
			isWorking_ = true;

			workThread_ = std::thread([&]()
			{
				const unsigned long interval = 1000 / fps_;

				while (isWorking_)
				{
					using namespace std::chrono;

					// 当前帧的开始执行时间
					time_point<system_clock, milliseconds> beginTime = time_point_cast<milliseconds>(system_clock::now());


					// 更新调度
					{
						lock_guard locker(scheduleLocker_);
						for (auto iter = updateFuncs_.begin(); iter != updateFuncs_.end(); ++iter)
						{
							UpdateFunc func = *iter;
							cosc_.StartCoroutine([func]()
								{
									func.f();
								});
						}
					}


					// 定时调度
					{
						lock_guard locker(scheduleLocker_);
						for (auto iter = scheduleFuncs_.begin(); iter != scheduleFuncs_.end();)
						{
							if ((beginTime - iter->timestamp).count() >= static_cast<long>(iter->duration))
							{
								iter->timestamp = beginTime;
								
								ScheduleFunc func = *iter;

								cosc_.StartCoroutine([func]()
									{
										func.f();
									});

								if (iter->isOnce)
								{
									iter = scheduleFuncs_.erase(iter);
									continue;
								}
							}

							++iter;
						}
					}

					// 网络调度
					while (true)
					{
						NetRequest request;

						// 获取网络请求
						{
							std::unique_lock<std::mutex> locker(netRequestLocker_);
							if (!netRequests_.empty())
							{
								request = std::move(netRequests_.front());
								netRequests_.pop();
							}
							else
							{
								// 尝试等待网络请求到来
								time_point<system_clock, milliseconds> stopTime = time_point_cast<milliseconds>(system_clock::now());
								long long waste = (stopTime - beginTime).count();
								if (waste < interval)
								{
									if (std::cv_status::timeout == netRequestNotify_.wait_for(locker, std::chrono::milliseconds(interval - waste)))
									{
										// 等待超时，退出网络调度
										break;
									}
									else
									{
										// 等待成功，再次获取网络请求
										continue;
									}
								}
								else
								{
									// 当前帧消耗完了时间片，退出网络调度
									break;
								}
							}
						}

						// 处理网络请求
						if (request.session != nullptr)
						{
							cosc_.StartCoroutine([&,request]()
								{
									if (dispatch_)
									{
										dispatch_(request.session, request.msgId, request.data);
									}

									if (request.finishCallback)
									{
										request.finishCallback();
									}
								});
						}

						// 检测当前帧是否还有时间片继续执行
						time_point<system_clock, milliseconds> stopTime = time_point_cast<milliseconds>(system_clock::now());
						if ((stopTime - beginTime).count() >= static_cast<long>(interval))
						{
							break;
						}
					}
				}
			});
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

