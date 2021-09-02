/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file scheduler.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_SCHEDULER_H
#define __DAXIA_NET_SERVER_SCHEDULER_H

#include <memory>
#include <mutex>
#include <queue>
#include <chrono>
#include "common/buffer.h"

namespace daxia
{
	namespace net
	{
		class Session;

		// �����ࡣ������µ��ȡ���ʱ���ȡ�����������ȡ�
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
			// ��������
			struct NetRequest
			{
				std::shared_ptr<Session> session;
				int msgId;
				common::Buffer data;
				std::function<void()> finishCallback;

				NetRequest(){}
				NetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer data, std::function<void()> finishCallback)
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
			void PushNetRequest(std::shared_ptr<Session> session, int msgId, const common::Buffer data, std::function<void()> finishCallback = nullptr);
			void Run(bool enableFps);
			void Stop();
		private:
			long long makeScheduleID();
			void runAsFps();
			void runAsNoFps();
			void asyncWaitCB(scheduleFunc func, long long id, long long duration, const boost::system::error_code& ec);
		private:
			// ���º���
			struct UpdateFunc
			{
				long long id;
				std::function<void()> f;
			};

			// ��ʱ������
			struct ScheduleFunc : public UpdateFunc
			{
				unsigned long duration;
				bool isOnce;
				std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timestamp;
			};
		private:
			unsigned long fps_;
			bool enableFps_;
			std::vector<UpdateFunc> updateFuncs_;
			std::vector<ScheduleFunc> scheduleFuncs_;
			std::mutex scheduleLocker_;
			bool isWorking_;
			std::thread workThread_;
			std::mutex netRequestLocker_;
			std::queue<NetRequest> netRequests_;
			netDispatchFunc	dispatch_;
			long long nextScheduleID_;
			std::mutex nextScheduleIDLocker_;
			boost::asio::io_service logicIoService_;
			std::vector<std::thread> logicThreads_;
			std::map<long long, boost::asio::deadline_timer*> timers_;
		};
	}// namespace net
}// namespace daxia
#endif // !__DAXIA_NET_SERVER_SCHEDULER_H

