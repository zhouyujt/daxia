/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file server.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 * 一个通用的服务器框架
 *
 */

#ifndef __DAXIA_NET_SERVER_H
#define __DAXIA_NET_SERVER_H

#include <memory>
#include "common/parser.h"
#include "common/define.h"
#include "controller.h"
#include "scheduler.h"
#include "router.h"

namespace daxia
{
	namespace net
	{
		class Server
		{
		public:
			Server();
			~Server();
		public:
			void Run(short port, common::Protocol protcol = common::Protocol_TCP, const char* root = nullptr);
			void Stop();
			void SetParser(std::shared_ptr<common::Parser> parser);
			void Handle(int msgId, std::shared_ptr<Controller> controller);
			void Handle(const char* url, std::shared_ptr<HttpController> controller);
			void EnableCheckHeartbeat(unsigned long milliseconds);
			void SetFPS(unsigned long fps);
			long long ScheduleUpdate(Scheduler::scheduleFunc func);
			long long Schedule(Scheduler::scheduleFunc func, unsigned long duration);
			long long ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration);
			void UnscheduleUpdate(long long scheduleID);
			void Unschedule(long long scheduleID);
			void UnscheduleAll();
			SessionsManager& GetSessionManager();
		private:
			Router router_;
		};
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_SERVER_H