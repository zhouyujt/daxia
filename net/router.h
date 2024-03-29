/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file router.h
 * \author 漓江里的大虾
 * \date 三月 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_ROUTER_H
#define __DAXIA_NET_SERVER_ROUTER_H

#ifdef _WIN32
#include <sdkddkver.h> // boost/asio.hpp需要包含此文件
#endif

#include <memory>
#include <boost/asio.hpp>
#include "sessions_manager.h"
#include "common/parser.h"
#include "common/http_parser.h"
#include "scheduler.h"

namespace boost
{
	namespace asio
	{
		namespace ssl
		{
			template <typename Stream>
			class stream;

			class context;
		}
	}
}

namespace daxia
{
	namespace net
	{
		class Controller;
		class HttpController;
		class Session;

		// 消息路由类
		class Router : public SessionsManager
		{
		public:
			typedef boost::asio::ip::tcp::endpoint endpoint;
			typedef boost::asio::ip::tcp::socket socket;
			typedef std::shared_ptr<socket> socket_ptr;
			typedef boost::asio::ssl::stream<socket> sslsocket;
			typedef std::shared_ptr<sslsocket> sslsocket_ptr;
			typedef boost::asio::ip::tcp::acceptor acceptor;
			typedef std::shared_ptr<acceptor> acceptor_ptr;
			typedef boost::system::error_code error_code;
		public:
			Router();
			~Router();
		public:
			bool RunAsTCP(short port);
			bool RunAsUDP(short port);
			bool RunAsWebsocket(short port, const daxia::string& path);
			bool RunAsHTTP(short port, const daxia::string& root);
			bool RunAsHTTPS(short port, const daxia::string& root, const daxia::string& pubCert, const daxia::string& priKey);
			void SetParser(std::shared_ptr<common::Parser> parser);
			void Stop();
			void Handle(int msgID, std::shared_ptr<Controller> controller, bool useCoroutine);
			void Handle(const char* url, std::shared_ptr<HttpController> controller, bool useCoroutine);
			void EnableCheckHeartbeat(unsigned long milliseconds);
			Scheduler& GetScheduler();
		private:
			void dispatchMessage(std::shared_ptr<Session>, int msgID, const common::Buffer& data);
			void dispatchHttpMessage(std::shared_ptr<Session>, int msgID, const common::Buffer& data);
			int getCoreCount() const;
			void onAccept(socket_ptr, const error_code&);
			void onAcceptSSL(sslsocket_ptr, const error_code&);
			void onMessage(const boost::system::error_code& err, long long sessionId, int msgId, const common::Buffer& msg);
		private:
			std::unordered_map<int, std::shared_ptr<Controller>> controllers_;
			std::unordered_map<std::string, std::shared_ptr<HttpController>> httpControllers_;
			boost::asio::io_service ios_;
			acceptor_ptr acceptor_;
			std::shared_ptr<common::Parser> parser_;
			std::vector<std::thread> ioThreads_;
			Scheduler scheduler_;
			long long heartbeatSchedulerId_;
			long long nextSessionId_;
			std::mutex sessionIdLocker_;
			daxia::string httpRoot_;
			std::shared_ptr<boost::asio::ssl::context> sslctx_;
			std::mutex controllLoker_;
		};
	}// namespace net
}// namespace daxia
#endif // !__DAXIA_NET_SERVER_ROUTER_H

