/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file client.h
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 */

#ifndef __DAXIA_NET_CLIENT_H
#define __DAXIA_NET_CLIENT_H

#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include "common/basic_session.h"
#include "common/buffer.h"

namespace daxia
{
	namespace net
	{
		// 客户端类
		class Client : public common::BasicSession
		{
		public:
			typedef boost::asio::ip::tcp::endpoint endpoint;
			typedef boost::asio::ip::tcp::socket socket;
			typedef std::function<void(int, const boost::system::error_code&, const common::Buffer& data)> handler;
			typedef std::lock_guard<std::mutex> lock_guard;
			typedef std::chrono::time_point <std::chrono::system_clock, std::chrono::milliseconds> timepoint;
			typedef std::function<void()> scheduleFunc;
		public:
			Client();
			~Client();
			Client(const Client&) = delete;
		protected:
			virtual void onPacket(const boost::system::error_code& error, int msgId, const common::Buffer& buffer) override;
		public:
			void Handle(int msgId, handler h);
			void EnableHeartbeat(unsigned long milliseconds);
			void Connect(const char* ip, short port);
			void Connect(const wchar_t* ip, short port);
			long long Schedule(scheduleFunc func, unsigned long firstDuration, unsigned long loopDuration);
			long long ScheduleOnce(scheduleFunc func, unsigned long duration);
			void Unschedule(long long scheduleID);
			void UnscheduleAll();
			static daxia::string GetLocalIpv4();
			static daxia::string GetIpv4Info(const char* ip);
		private:
			struct LogicMessage
			{
				boost::system::error_code error;
				int msgID;
				common::Buffer buffer;

				LogicMessage(){}

				LogicMessage(const boost::system::error_code& error, int msgID, const common::Buffer& buffer)
					: error(error)
					, msgID(msgID)
					, buffer(buffer)
				{

				}
			};
		private:
			void doConnect();
			void hearbeat();
			void clearMessage();
			void pushLogciMessage(const LogicMessage& msg);
			void asyncWaitCB(scheduleFunc func, long long id, long long duration, const boost::system::error_code& ec);
		private:
			class initHelper
			{
				friend Client;
			public:
				initHelper();
				~initHelper();
			private:
				void startLogicThread();
				void stopLogicThread();
				void startIoThread();
				void stopIoThread();
				int getCoreCount() const;
			private:
				boost::asio::io_service netIoService_;
				boost::asio::io_service logicIoService_;
				std::vector<std::thread> ioThreads_;
				std::vector<std::thread> logicThreads_;
			};
		private:
			initHelper* initHelper_;
			std::shared_ptr<common::Parser> parser_;
			std::map<int, handler> handler_;
			unsigned long hearbeatInterval_;
			long long heartbeatSchedulerId_;
			endpoint endpoint_;
			std::queue<LogicMessage> logicMsgs_;
			std::mutex logicMsgLocker_;
			std::mutex scheduleLocker_;
			std::mutex handlerLocker_;
			std::map<long long, boost::asio::deadline_timer*> timers_;
			long long nextTimerId_;
		};
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_CLIENT_H
