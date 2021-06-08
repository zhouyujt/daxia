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

#ifndef __DAXIA_DXG_SERVER_SCHEDULER_H
#define __DAXIA_DXG_SERVER_SCHEDULER_H

#include <memory>
#include <mutex>
#include <queue>
#include <chrono>
#include "common/shared_buffer.h"

namespace daxia
{
	namespace dxg
	{
		class Session;

		// 调度类。负责更新调度、定时调度、网络请求调度。
		class Scheduler
		{
		public:
			typedef std::function<void()> scheduleFunc;
			typedef std::function<void(std::shared_ptr<Session>, int, const common::shared_buffer)> netDispatchFunc;
			typedef std::lock_guard<std::mutex> lock_guard;
		public:
			Scheduler();
			~Scheduler();
		public:
			// 网络请求
			struct NetRequest
			{
				std::shared_ptr<Session> session;
				int msgId;
				common::shared_buffer data;
				std::function<void()> finishCallback;

				NetRequest(){}
				NetRequest(std::shared_ptr<Session> session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback)
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
			void PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::shared_buffer data, std::function<void()> finishCallback = nullptr);
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
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_DXG_SERVER_SCHEDULER_H

