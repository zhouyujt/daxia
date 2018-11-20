/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file scheduler.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 * 
 */

#ifndef __DAXIA_DXG_SERVER_SCHEDULER_HPP
#define __DAXIA_DXG_SERVER_SCHEDULER_HPP

#include <chrono>
#include <thread>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <daxia/dxg/server/session.hpp>
#include <daxia/dxg/common/shared_buffer.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			// 调度类。负责更新调度、定时调度、网络请求调度。
			class Scheduler
			{
			public:
				typedef std::function<void()> scheduleFunc;
				typedef std::function<void(Session::ptr,int,const common::shared_buffer)> netDispatchFunc;
				typedef std::lock_guard<std::mutex> lock_guard;
			public:
				Scheduler();
				~Scheduler();
			public:
				// 网络请求
				struct NetRequest
				{
					Session::ptr session;
					int msgId;
					common::shared_buffer data;
					std::function<void()> finishCallback;

					NetRequest(){}
					NetRequest(Session::ptr session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback)
						: session(session)
						, msgId(msgId)
						, data(data)
						, finishCallback(finishCallback)
					{
					}
				};
			public:
				void SetFPS(unsigned long fps);
				long long ScheduleUpdate(scheduleFunc func);
				long long Schedule(scheduleFunc func, unsigned long duration);
				long long ScheduleOnce(scheduleFunc func, unsigned long duration);
				void UnscheduleUpdate(long long scheduleID);
				void Unschedule(long long scheduleID);
				void UnscheduleAll();
				void SetNetDispatch(netDispatchFunc func);
				void PushNetRequest(Session::ptr session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback = nullptr);
				void Run();
				void Stop();
			private:
				long long makeScheduleID();
			private:
				// 更新函数
				struct UpdateFunc
				{
					long long id;
					std::function<void()> f;
				};

				// 定时器函数
				struct ScheduleFunc : public UpdateFunc
				{
					unsigned long duration;
					bool isOnce;
					std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timestamp;
				};
			private:
				unsigned long fps_;
				std::vector<UpdateFunc> updateFuncs_;
				std::vector<ScheduleFunc> scheduleFuncs_;
				std::mutex scheduleLocker_;
				bool isWorking_;
				std::thread workThread_;
				std::mutex netRequestLocker_;
				std::queue<NetRequest> netRequests_;
				netDispatchFunc	dispatch_;
			private:
				long long nextScheduleID_;
				std::mutex nextScheduleIDLocker_;
			};

			inline long long Scheduler::makeScheduleID()
			{
				lock_guard locker(nextScheduleIDLocker_);

				return nextScheduleID_++;
			}

			//////////////////////////////////////////////////////////////////////////
			inline Scheduler::Scheduler()
				: fps_(20)
				, isWorking_(false)
			{
			}

			inline Scheduler::~Scheduler()
			{
				Stop();
			}

			inline void Scheduler::SetFPS(unsigned long fps)
			{
				fps_ = fps;
			}

			inline long long Scheduler::ScheduleUpdate(scheduleFunc func)
			{
				lock_guard locker(scheduleLocker_);

				UpdateFunc updateFunc;
				updateFunc.id = makeScheduleID();
				updateFunc.f = func;

				updateFuncs_.push_back(updateFunc);

				return updateFunc.id;
			}

			inline long long Scheduler::Schedule(scheduleFunc func, unsigned long duration)
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

			inline long long Scheduler::ScheduleOnce(scheduleFunc func, unsigned long duration)
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

			inline void Scheduler::UnscheduleUpdate(long long scheduleID)
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

			inline void Scheduler::Unschedule(long long scheduleID)
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

			inline void Scheduler::UnscheduleAll()
			{
				lock_guard locker(scheduleLocker_);

				updateFuncs_.clear();
				scheduleFuncs_.clear();
			}

			inline void Scheduler::SetNetDispatch(netDispatchFunc func)
			{
				dispatch_ = func;
			}

			inline void Scheduler::PushNetRequest(Session::ptr session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback)
			{
				lock_guard locker(netRequestLocker_);
				netRequests_.push(NetRequest(session, msgId, data, finishCallback));
			}

			inline void Scheduler::Run()
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

			inline void Scheduler::Stop()
			{
				isWorking_ = false;

				if (workThread_.joinable())
				{
					workThread_.join();
				}
			}

		}// namespace server
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_DXG_SERVER_SCHEDULER_HPP

