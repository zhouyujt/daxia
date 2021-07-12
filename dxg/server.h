/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file dxg.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 * һ��ͨ�õ���Ϸ���������
 *
 */

#ifndef __DAXIA_DXG_SERVER_H
#define __DAXIA_DXG_SERVER_H

#include <memory>
#include "common/parser.h"
#include "common/define.h"
#include "controller.h"
#include "scheduler.h"
#include "router.h"

namespace daxia
{
	namespace dxg
	{
		class Server
		{
		public:
			Server();
			~Server();
		public:
			void Run(short port, common::Protocol protcol = common::Protocol_TCP);
			void Stop();
			void SetParser(std::shared_ptr<common::Parser> parser);
			void Handle(int msgId, std::shared_ptr<Controller> controller);
			void EnableCheckHeartbeat(unsigned long milliseconds);
			void SetFPS(unsigned long fps);
			long long ScheduleUpdate(Scheduler::scheduleFunc func);
			long long Schedule(Scheduler::scheduleFunc func, unsigned long duration);
			long long ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration);
			void UnscheduleUpdate(long long scheduleID);
			void Unschedule(long long scheduleID);
			void UnscheduleAll();
			const SessionsManager& GetSessionManager() const;
		private:
			std::string websocketPath_;
			Router router_;
		};
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_SERVER_H