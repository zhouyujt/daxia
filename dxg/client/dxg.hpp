/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file dxg.hpp
 * \author 漓江里的大虾
 * \date 十月 2018
 *
 */

#ifndef __DAXIA_DXG_CLIENT_DXG_HPP
#define __DAXIA_DXG_CLIENT_DXG_HPP

#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <boost/asio.hpp>
#include <daxia/dxg/common/basic_session.hpp>
#include <daxia/dxg/common/parser.hpp>
#include <daxia/encode/strconv.hpp>

#define DXG_CLIENT_HANDLER(id,error,date,len) [&](int id, const boost::system::error_code& error, const void* date, int len)

namespace daxia
{
	namespace dxg
	{
		namespace client
		{
			// 客户端类
			class Client : public common::BasicSession
			{
			public:
				typedef boost::asio::ip::tcp::endpoint endpoint;
				typedef boost::asio::ip::tcp::socket socket;
				typedef std::function<void(int, const boost::system::error_code&, const void*, int)> handler;
				typedef std::lock_guard<std::mutex> lock_guard;
				typedef std::chrono::time_point <std::chrono::system_clock, std::chrono::milliseconds> timepoint;
			public:
				Client();
				~Client();
			protected:
				virtual void onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer) override;
				virtual void onClose() override;
			public:
				void Handle(int msgId, handler h);
				void EnableHeartbeat(unsigned long milliseconds);
				void Connect(const char* ip, short port);
				void Connect(const wchar_t* ip, short port);
			private:
				struct LogicMessage
				{
					boost::system::error_code error;
					int msgID;
					common::shared_buffer buffer;

					LogicMessage(){}

					LogicMessage(const boost::system::error_code& error, int msgID, const common::shared_buffer& buffer)
						: error(error)
						, msgID(msgID)
						, buffer(buffer)
					{

					}
				};
			private:
				void doConnect();
				void hearbeat();
				void startLogicThread();
				void stopLogicThread();
				void clearMessage();
				void pushLogciMessage(const LogicMessage& msg);
			private:
				boost::asio::io_service netIoService_;
				boost::asio::io_service logicIoService_;
				common::shared_buffer buffer_;
				std::thread ioThread_;
				std::thread logicThread_;
				std::shared_ptr<common::Parser> parser_;
				std::map<int, handler> handler_;
				bool isIoWorking_;
				bool isLogicWorking_;
				std::queue<LogicMessage> logicMsgs_;
				std::mutex logicMsgLocker_;
				unsigned long hearbeatInterval_;
				endpoint endpoint_;
			};

			inline Client::Client()
				: buffer_(1024 * 8)
				, isIoWorking_(false)
				, isLogicWorking_(false)
				, hearbeatInterval_(0)
			{
				initSocket(BasicSession::socket_ptr(new socket(netIoService_)));
				parser_ = common::Parser::ptr(new common::DefaultParser);
				startLogicThread();
			}

			inline Client::~Client()
			{
				stopLogicThread();
				Close();

				// 基类的sock_析构时依赖本类的netIoService_,这里使之提前析构
				initSocket(BasicSession::socket_ptr());
			}

			inline void Client::onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer)
			{
				pushLogciMessage(LogicMessage(error, msgId, buffer));
			}

			inline void Client::onClose()
			{
				isIoWorking_ = false;
				netIoService_.stop();
				if (ioThread_.joinable())
				{
					ioThread_.join();
				}

				clearMessage();
			}

			inline void Client::Handle(int msgId, handler h)
			{
				handler_[msgId] = h;
			}

			inline void Client::EnableHeartbeat(unsigned long milliseconds)
			{
				hearbeatInterval_ = milliseconds;
			}

			inline void Client::Connect(const char* ip, short port)
			{
				if (isIoWorking_) return;

				isIoWorking_ = true;
				ioThread_ = std::thread([&]()
				{
					boost::asio::deadline_timer timer(netIoService_, boost::posix_time::milliseconds(100));
					while (isIoWorking_)
					{
						// 保持netIoService_.run不退出
						timer.async_wait([&](const boost::system::error_code& ec)
						{
							timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(100));
						});

						netIoService_.run();
						netIoService_.reset();
					}
				});

				endpoint_ = endpoint(boost::asio::ip::address::from_string(ip), port);
				doConnect();
			}

			inline void Client::Connect(const wchar_t* ip, short port)
			{
				std::string ip2 = daxia::encode::Unicode2Ansi(ip);
				Connect(ip2.c_str(), port);
			}

			inline void Client::doConnect()
			{
				getSocket()->async_connect(endpoint_, [&](const boost::system::error_code& ec)
				{
					pushLogciMessage(LogicMessage(ec, common::DefMsgID_Connect, common::shared_buffer()));

					if (!ec)
					{
						postRead();
					}
				});
			}

			inline void Client::hearbeat()
			{
				using namespace std::chrono;

				if (hearbeatInterval_ != 0)
				{
					time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());
					if ((now - GetLastWriteTime()).count() >= hearbeatInterval_)
					{
						WriteMessage(nullptr, 0);
					}
				}
			}

			inline void Client::startLogicThread()
			{
				isLogicWorking_ = true;
				logicThread_ = std::thread([&]()
				{
					boost::asio::deadline_timer timer(logicIoService_, boost::posix_time::milliseconds(1000));
					while (isLogicWorking_)
					{
						// 保持logicIoService_.run不退出
						timer.async_wait([&](const boost::system::error_code& ec)
						{
							// 心跳
							hearbeat();
							timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(1000));
						});

						logicIoService_.run();
						logicIoService_.reset();
					}
				});
			}

			inline void Client::stopLogicThread()
			{
				clearMessage();

				isLogicWorking_ = false;
				if (logicThread_.joinable())
				{
					logicThread_.join();
				}
			}

			inline void Client::clearMessage()
			{
				// clear messages_
				logicMsgLocker_.lock();
				std::queue<LogicMessage> empty;
				logicMsgs_.swap(empty);
				logicMsgLocker_.unlock();
			}

			inline void Client::pushLogciMessage(const LogicMessage& msg)
			{
				lock_guard locker(logicMsgLocker_);
				logicMsgs_.push(msg);

				logicIoService_.post([&]()
				{
					// 分发消息
					LogicMessage msg;
					bool hasMsg = false;
					logicMsgLocker_.lock();
					if (!logicMsgs_.empty())
					{
						hasMsg = true;
						msg = logicMsgs_.front();
						logicMsgs_.pop();
					}
					logicMsgLocker_.unlock();

					if (hasMsg)
					{
						if (msg.msgID == common::DefMsgID_DisConnect)
						{
							Close();
						}

						auto iter = handler_.find(msg.msgID);
						if (iter != handler_.end())
						{
							iter->second(msg.msgID, msg.error, msg.buffer.get(), msg.buffer.size());
						}
						else
						{
							auto iter = handler_.find(common::DefMsgID_UnHandle);
							if (iter != handler_.end())
							{
								iter->second(msg.msgID, msg.error, msg.buffer.get(), msg.buffer.size());
							}
						}
					}
				});
			}

		}// namespace client
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_CLIENT_DXG_HPP
