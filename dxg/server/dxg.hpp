/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file dxg.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 * 
 * 一个通用的游戏服务器框架
 *
 */

#ifndef __DAXIA_DXG_SERVER_DXG_HPP
#define __DAXIA_DXG_SERVER_DXG_HPP

#include <string>
#include <daxia/dxg/common/define.hpp>
#include <daxia/dxg/server/client.hpp>
#include <daxia/dxg/server/client_manager.hpp>
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
				Server() : router_(*this){}
				~Server(){}
			public:
				void Run(short port, common::Protocol protcol = common::Protocol_TCP);
				void Stop();
				void SetParser(common::Parser::parser_ptr parser);
				void Handle(int msgID, Controller::controller_ptr controller);
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
			void Server::Run(short port, common::Protocol protcol)
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

			void Server::Stop()
			{
				router_.Stop();
			}

			void Server::SetParser(common::Parser::parser_ptr parser)
			{
				router_.SetParser(parser);
			}

			void Server::Handle(int msgID, Controller::controller_ptr controller)
			{
				router_.Handle(msgID, controller);
			}

			void Server::EnableHeartbeat(unsigned long milliseconds)
			{
				router_.EnableHeartbeat(milliseconds);
			}

			void Server::SetFPS(unsigned long fps)
			{
				router_.GetScheduler().SetFPS(fps);
			}

			long long Server::ScheduleUpdate(Scheduler::scheduleFunc func)
			{
				return router_.GetScheduler().ScheduleUpdate(func);
			}

			long long Server::Schedule(Scheduler::scheduleFunc func, unsigned long duration)
			{
				return router_.GetScheduler().Schedule(func, duration);
			}

			long long Server::ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration)
			{
				return router_.GetScheduler().ScheduleOnce(func, duration);
			}

			void Server::UnscheduleUpdate(long long scheduleID)
			{
				router_.GetScheduler().UnscheduleUpdate(scheduleID);
			}

			void Server::Unschedule(long long scheduleID)
			{
				router_.GetScheduler().Unschedule(scheduleID);
			}

			void Server::UnscheduleAll()
			{
				router_.GetScheduler().UnscheduleAll();
			}
		}// namespace server
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_SERVER_DXG_HPP