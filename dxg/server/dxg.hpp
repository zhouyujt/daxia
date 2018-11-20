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

#ifndef __DAXIA_DXG_SERVER_DXG_HPP
#define __DAXIA_DXG_SERVER_DXG_HPP

#include <string>
#include <daxia/dxg/common/define.hpp>
#include <daxia/dxg/server/session.hpp>
#include <daxia/dxg/server/sessions_manager.hpp>
#include <daxia/dxg/server/router.hpp>
#include <daxia/dxg/server/controller.hpp>
#include <daxia/dxg/common/parser.hpp>
#include <daxia/dxg/common/shared_buffer.hpp>

#if !defined (_WIN32) && !defined (_WIN64)  
#include <sysconf.h>  
#else  
#include <windows.h>  
#endif 

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			class Server
			{
			public:
				Server(){}
				~Server(){ Stop(); }
			public:
				void Run(short port, common::Protocol protcol = common::Protocol_TCP);
				void Stop();
				void SetParser(common::Parser::ptr parser);
				void Handle(int msgId, Controller::ptr controller);
				void EnableHeartbeat(unsigned long milliseconds);
				void SetFPS(unsigned long fps);
				long long ScheduleUpdate(Scheduler::scheduleFunc func);
				long long Schedule(Scheduler::scheduleFunc func, unsigned long duration);
				long long ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration);
				void UnscheduleUpdate(long long scheduleID);
				void Unschedule(long long scheduleID);
				void UnscheduleAll();
			private:
				std::string websocketPath_;
				Router router_;
			};

			//////////////////////////////////////////////////////////////////////////
			inline void Server::Run(short port, common::Protocol protcol)
			{
				switch (protcol)
				{
				case common::Protocol_TCP:
					router_.RunAsTCP(port);
					break;
				case common::Protocol_UDP:
					router_.RunAsUDP(port);
					break;
				case common::Protocol_Websocket:
					router_.RunAsWebsocket(port, websocketPath_);
					break;
				case common::Protocol_HTTP:
					router_.RunAsHTTP(port);
					break;
				default:
					break;
				}
			}

			inline void Server::Stop()
			{
				router_.Stop();
			}

			inline void Server::SetParser(common::Parser::ptr parser)
			{
				router_.SetParser(parser);
			}

			inline void Server::Handle(int msgId, Controller::ptr controller)
			{
				router_.Handle(msgId, controller);
			}

			inline void Server::EnableHeartbeat(unsigned long milliseconds)
			{
				router_.EnableHeartbeat(milliseconds);
			}

			inline void Server::SetFPS(unsigned long fps)
			{
				router_.GetScheduler().SetFPS(fps);
			}

			inline long long Server::ScheduleUpdate(Scheduler::scheduleFunc func)
			{
				return router_.GetScheduler().ScheduleUpdate(func);
			}

			inline long long Server::Schedule(Scheduler::scheduleFunc func, unsigned long duration)
			{
				return router_.GetScheduler().Schedule(func, duration);
			}

			inline long long Server::ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration)
			{
				return router_.GetScheduler().ScheduleOnce(func, duration);
			}

			inline void Server::UnscheduleUpdate(long long scheduleID)
			{
				router_.GetScheduler().UnscheduleUpdate(scheduleID);
			}

			inline void Server::Unschedule(long long scheduleID)
			{
				router_.GetScheduler().Unschedule(scheduleID);
			}

			inline void Server::UnscheduleAll()
			{
				router_.GetScheduler().UnscheduleAll();
			}

		}// namespace server
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_SERVER_DXG_HPP