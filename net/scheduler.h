/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file scheduler.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_SCHEDULER_H
#define __DAXIA_NET_SERVER_SCHEDULER_H

#include <memory>
#include <mutex>
#include <queue>
#include <chrono>
#include "common/buffer.h"
#include "../system/coroutine.h"

namespace daxia
{
	namespace net
	{
		class Session;

		// 调度类。负责更新调度、定时调度、网络请求调度。
		class Scheduler
		{
		public:
			typedef std::function<void()> scheduleFunc;
			typedef std::function<void(std::shared_ptr<Session>, int, const common::Buffer)> netDispatchFunc;
			typedef std::lock_guard<std::mutex> lock_guard;
		public:
			Scheduler();
			~Scheduler();
		public:
			// 网络请求
			struct NetRequest
			{
				NetRequest(const NetRequest& request)
				{
					session = request.session;
					msgId = request.msgId;
					data = request.data;
					finishCallback = request.finishCallback;
				}

				NetRequest(NetRequest&& request)
				{
					session.swap(request.session);
					msgId = request.msgId;
					data = std::move(request.data);
					finishCallback.swap(request.finishCallback);
				}
					
				NetRequest& operator=(NetRequest& request)
				{
					session = request.session;
					msgId = request.msgId;
					data = request.data;
					finishCallback = request.finishCallback;

					return *this;
				}

				NetRequest& operator=(NetRequest&& request)
				{
					session.swap(request.session);
					msgId = request.msgId;
					data = std::move(request.data);
					finishCallback.swap(request.finishCallback);

					return *this;
				}

				std::shared_ptr<Session> session;
				int msgId;
				common::Buffer data;
				std::function<void()> finishCallback;

				NetRequest(){}
				NetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer& data, const std::function<void()>& finishCallback)
					: session(session)
					, msgId(msgId)
					, data(data)
					, finishCallback(finishCallback)
				{
				}
			};
		public:
			void SetFps(unsigned long fps);
			long long ScheduleUpdate(scheduleFunc func);
			long long Schedule(scheduleFunc func, unsigned long duration);
			long long ScheduleOnce(scheduleFunc func, unsigned long duration);
			void UnscheduleUpdate(long long scheduleID);
			void Unschedule(long long scheduleID);
			void UnscheduleAll();
			void SetNetDispatch(netDispatchFunc func);
			void PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer& data, std::function<void()> finishCallback = nullptr);
			void Run();
			void Stop();
		private:
			long long makeScheduleID();
			void run();
			void asyncWaitCB(scheduleFunc func, long long id, long long duration, const boost::system::error_code& ec);
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
			std::condition_variable netRequestNotify_;
			std::queue<NetRequest> netRequests_;
			netDispatchFunc	dispatch_;
			long long nextScheduleID_;
			std::mutex nextScheduleIDLocker_;
			std::map<long long, boost::asio::deadline_timer*> timers_;
			daxia::system::CoScheduler cosc_;
		};
	}// namespace net
}// namespace daxia
#endif // !__DAXIA_NET_SERVER_SCHEDULER_H

