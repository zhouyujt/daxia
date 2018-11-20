/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file router.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
 * 
 */

#ifndef __DAXIA_DXG_SERVER_ROUTER_HPP
#define __DAXIA_DXG_SERVER_ROUTER_HPP

#include <thread>
#include <queue>
#include <boost/asio.hpp>
#include <daxia/dxg/server/controller.hpp>
#include <daxia/dxg/server/scheduler.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			// 消息路由类
			class Router
			{
			public:
				typedef boost::asio::ip::tcp::endpoint endpoint;
				typedef boost::asio::ip::tcp::socket socket;
				typedef std::shared_ptr<socket> socket_ptr;
				typedef boost::asio::ip::tcp::acceptor acceptor;
				typedef std::shared_ptr<acceptor> acceptor_ptr;
				typedef boost::system::error_code error_code;
			public:
				Router();
				~Router();
			public:
				void RunAsTCP(short port);
				void RunAsUDP(short port);
				void RunAsWebsocket(short port, const std::string& path);
				void RunAsHTTP(short port);
				void SetParser(common::Parser::ptr parser);
				void Stop();
				void Handle(int msgID, Controller::ptr controller);
				void EnableHeartbeat(unsigned long milliseconds);
				Scheduler& GetScheduler();
			private:
				void dispatchMessage(Session::ptr, int msgID, const common::shared_buffer data);
				int getCoreCount() const;
				void onAccept(socket_ptr, const error_code&);
				void onMessage(const boost::system::error_code& err, long long sessionId, int msgId, common::shared_buffer msg);
			private:
				std::map<int, Controller::ptr> controllers_;
				boost::asio::io_service ios_;
				acceptor_ptr acceptor_;
				common::Parser::ptr parser_;
				SessionsManager::ptr sessionsMgr_;
				std::vector<std::thread> ioThreads_;
				Scheduler scheduler_;
			};

			//////////////////////////////////////////////////////////////////////////
			inline Router::Router()
			{
				sessionsMgr_ = SessionsManager::ptr(new SessionsManager(scheduler_));
				parser_ = common::Parser::ptr(new common::DefaultParser);
			}

			inline Router::~Router()
			{

			}

			inline void Router::RunAsTCP(short port)
			{
				endpoint ep(boost::asio::ip::tcp::v4(), port);
				acceptor_ = acceptor_ptr(new acceptor(ios_, ep));

				socket_ptr socketSession(new socket(ios_));
				acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

				// 启动I/O线程
				int coreCount = getCoreCount();
				for (int i = 0; i < coreCount * 2; ++i)
				{
					ioThreads_.push_back(
						std::thread([=]()
					{
						ios_.run();
					}));
				}

				// 启动调度器
				scheduler_.SetNetDispatch(std::bind(&Router::dispatchMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				scheduler_.Run();
			}

			inline void Router::RunAsUDP(short port)
			{
			}

			inline void Router::RunAsWebsocket(short port, const std::string& path)
			{
			}

			inline void Router::RunAsHTTP(short port)
			{

			}

			inline void Router::SetParser(common::Parser::ptr parser)
			{
				parser_ = parser;
			}

			inline void Router::Stop()
			{
				// 结束I/O线程
				ios_.stop();
				for (size_t i = 0; i < ioThreads_.size(); ++i)
				{
					if (ioThreads_[i].joinable())
					{
						ioThreads_[i].join();
					}
				}

				// 结束调度器
				scheduler_.Stop();
			}

			inline void Router::Handle(int msgID, Controller::ptr controller)
			{
				controllers_[msgID] = controller;
			}

			inline void Router::EnableHeartbeat(unsigned long milliseconds)
			{
				sessionsMgr_->EnableCheckHeartbeat(milliseconds);
			}

			inline Scheduler& Router::GetScheduler()
			{
				return scheduler_;
			}

			inline void Router::dispatchMessage(Session::ptr client, int msgID, const common::shared_buffer data)
			{
				auto iter = controllers_.find(msgID);
				if (iter != controllers_.end())
				{
					iter->second->Proc(msgID, client, sessionsMgr_, data);
				}
				else
				{
					auto iter = controllers_.find(common::DefMsgID_UnHandle);
					if (iter != controllers_.end())
					{
						iter->second->Proc(msgID, client, sessionsMgr_, data);
					}
				}
			}

			inline int Router::getCoreCount() const
			{
				int count = 1; // 至少一个

#if !defined (_WIN32) && !defined (_WIN64) 
				count = sysconf(_SC_NPROCESSORS_CONF);
#else
				SYSTEM_INFO si;
				GetSystemInfo(&si);
				count = si.dwNumberOfProcessors;
#endif  

				return count;
			}

			inline void Router::onAccept(socket_ptr sock, const error_code& err)
			{
				if (!err)
				{
					socket_ptr socketSession(new socket(ios_));
					acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

					auto session = sessionsMgr_->AddSession(sock, parser_, std::bind(&Router::onMessage,
						this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3,
						std::placeholders::_4));

					scheduler_.PushNetRequest(session, common::DefMsgID_Connect, common::shared_buffer());
				}
			}

			inline void Router::onMessage(const boost::system::error_code& err, long long sessionId, int msgId, common::shared_buffer msg)
			{
				if (err || msgId == common::DefMsgID_DisConnect)
				{
					scheduler_.PushNetRequest(sessionsMgr_->GetSession(sessionId), common::DefMsgID_DisConnect, common::shared_buffer(), [&, sessionId]()
					{
						sessionsMgr_->DeleteSession(sessionId);
					});
				}
				else
				{
					scheduler_.PushNetRequest(sessionsMgr_->GetSession(sessionId), msgId, msg);
				}
			}

		}// namespace server
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_DXG_SERVER_ROUTER_HPP

