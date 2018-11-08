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
				Router(Server& server);
				~Router();
			public:
				void RunAsTCP(short port);
				void RunAsUDP(short port);
				void RunAsWebsocket(short port, const std::string& path);
				void RunAsHTTP(short port);
				void SetParser(common::Parser::parser_ptr parser);
				void Stop();
				void Handle(int msgID, Controller::controller_ptr controller);
				void EnableHeartbeat(unsigned long milliseconds);
				Scheduler& GetScheduler();
			private:
				void dispatchMessage(Client::client_ptr, int msgID, const common::shared_buffer data);
				int getCoreCount() const;
				void onAccept(socket_ptr, const error_code&);
				void onMessage(const boost::system::error_code& err, long long clientID, int msgID, common::shared_buffer msg);
			private:
				std::map<int, Controller::controller_ptr> controllers_;
				boost::asio::io_service ios_;
				acceptor_ptr acceptor_;
				common::Parser::parser_ptr parser_;
				ClientManager::clientMgr_ptr clientMgr_;
				std::vector<std::thread> IOThreads_;
				Scheduler scheduler_;
				Server& server_;
			};

			//////////////////////////////////////////////////////////////////////////
			Router::Router(Server& server)
				: server_(server)
			{
				clientMgr_ = ClientManager::clientMgr_ptr(new ClientManager(scheduler_));
				parser_ = common::Parser::parser_ptr(new common::DefaultParser);
			}

			Router::~Router()
			{

			}

			void Router::RunAsTCP(short port)
			{
				endpoint ep(boost::asio::ip::tcp::v4(), port);
				acceptor_ = acceptor_ptr(new acceptor(ios_, ep));

				socket_ptr socketSession(new socket(ios_));
				acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

				// 启动I/O线程
				int coreCount = getCoreCount();
				for (int i = 0; i < coreCount * 2; ++i)
				{
					IOThreads_.push_back(
						std::thread([=]()
					{
						ios_.run();
					}));
				}

				// 启动调度器
				scheduler_.SetNetDispatch(std::bind(&Router::dispatchMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				scheduler_.Run();
			}

			void Router::RunAsUDP(short port)
			{
			}

			void Router::RunAsWebsocket(short port, const std::string& path)
			{
			}

			void Router::RunAsHTTP(short port)
			{

			}

			void Router::SetParser(common::Parser::parser_ptr parser)
			{
				parser_ = parser;
			}

			void Router::Stop()
			{
				// 结束I/O线程
				ios_.stop();
				for (size_t i = 0; i < IOThreads_.size(); ++i)
				{
					IOThreads_[i].join();
				}

				// 结束调度器
				scheduler_.Stop();
			}

			void Router::Handle(int msgID, Controller::controller_ptr controller)
			{
				controllers_[msgID] = controller;
			}

			void Router::EnableHeartbeat(unsigned long milliseconds)
			{
				clientMgr_->EnableCheckHeartbeat(milliseconds);
			}

			Scheduler& Router::GetScheduler()
			{
				return scheduler_;
			}

			void Router::dispatchMessage(Client::client_ptr client, int msgID, const common::shared_buffer data)
			{
				auto iter = controllers_.find(msgID);
				if (iter != controllers_.end())
				{
					iter->second->Proc(server_, client, clientMgr_,data);
				}
			}

			int Router::getCoreCount() const
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

			void Router::onAccept(socket_ptr sock, const error_code& err)
			{
				if (!err)
				{
					socket_ptr socketSession(new socket(ios_));
					acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

					ClientManager::client_ptr client(new Client(sock, parser_, std::bind(&Router::onMessage,
						this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3,
						std::placeholders::_4)));
					clientMgr_->AddClient(client);

					scheduler_.PushNetRequest(client, common::DefMsgID_Connect, common::shared_buffer());
				}
			}

			void Router::onMessage(const boost::system::error_code& err, long long clientID, int msgID, common::shared_buffer msg)
			{
				if (err || msgID == common::DefMsgID_DisConnect)
				{
					scheduler_.PushNetRequest(clientMgr_->GetClient(clientID), common::DefMsgID_DisConnect, common::shared_buffer(), [&, clientID]()
					{
						clientMgr_->DeleteClient(clientID);
					});
				}
				else
				{
					scheduler_.PushNetRequest(clientMgr_->GetClient(clientID), msgID, msg);
				}
			}
		}// namespace server
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_DXG_SERVER_ROUTER_HPP

