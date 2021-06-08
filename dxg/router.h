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

#ifndef __DAXIA_DXG_SERVER_ROUTER_H
#define __DAXIA_DXG_SERVER_ROUTER_H

#include <memory>
#include <boost/asio.hpp>
#include "common/parser.h"
#include "scheduler.h"

namespace daxia
{
	namespace dxg
	{
		class Controller;
		class Session;
		class SessionsManager;

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
			void SetParser(std::shared_ptr<common::Parser> parser);
			void Stop();
			void Handle(int msgID, std::shared_ptr<Controller> controller);
			void EnableHeartbeat(unsigned long milliseconds);
			Scheduler& GetScheduler();
		private:
			void dispatchMessage(std::shared_ptr<Session>, int msgID, const common::shared_buffer data);
			int getCoreCount() const;
			void onAccept(socket_ptr, const error_code&);
			void onMessage(const boost::system::error_code& err, long long sessionId, int msgId, common::shared_buffer msg);
		private:
			std::map<int, std::shared_ptr<Controller>> controllers_;
			boost::asio::io_service ios_;
			acceptor_ptr acceptor_;
			std::shared_ptr<common::Parser> parser_;
			std::shared_ptr<SessionsManager> sessionsMgr_;
			std::vector<std::thread> ioThreads_;
			Scheduler scheduler_;
		};
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_DXG_SERVER_ROUTER_H

