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
#include <daxia/dxg/common/define.hpp>
#include <daxia/dxg/common/parser.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace client
		{
			// 客户端类
			class Client
			{
			public:
				typedef boost::asio::ip::tcp::endpoint endpoint;
				typedef boost::asio::ip::tcp::socket socket;
				typedef std::function<void(const boost::system::error_code&, const void*, int)> handler;
				typedef std::lock_guard<std::mutex> lock_guard;
				typedef std::chrono::time_point <std::chrono::system_clock, std::chrono::milliseconds> timepoint;
			public:
				Client();
				~Client();
			public:
				void SetParser(common::Parser::parser_ptr parser);
				void Handle(int msgID, handler h);
				void EnableHeartbeat(unsigned long milliseconds);
				void Connect(const char* ip, short port);
				void Connect(const wchar_t* ip, short port);
				void WriteMessage(const void* date, int len);
			private:
				void doConnect();
				void doReconnect();
				void doWriteMessage();
				void onRead(const boost::system::error_code& err, size_t len);
				void hearbeat();
				void startLogicThread();
				void stopLogicThread();
			private:
				struct Message
				{
					boost::system::error_code error;
					int msgID;
					common::shared_buffer buffer;

					Message(){}

					Message(const boost::system::error_code& error, int msgID, const common::shared_buffer& buffer)
						: error(error)
						, msgID(msgID)
						, buffer(buffer)
					{

					}
				};
			private:
				boost::asio::io_service ios_;
				socket sock_;
				common::shared_buffer buffer_;
				std::thread IOThread_;
				std::thread logicThread_;
				std::shared_ptr<common::Parser> parser_;
				std::map<int, handler> handler_;
				std::mutex writeLocker_;
				std::queue<common::shared_buffer> writeBufferCache_;
				bool isWorking_;
				std::queue<Message> messages_;
				std::mutex msgLocker_;
				timepoint lastWriteTime_;
				unsigned long hearbeatInterval_;
				endpoint endpoint_;
			};

			Client::Client()
				: sock_(ios_)
				, buffer_(1024 * 8)
				, isWorking_(false)
				, hearbeatInterval_(0)
			{
				parser_ = common::Parser::parser_ptr(new common::DefaultParser);
			}

			Client::~Client()
			{
				ios_.stop();
				IOThread_.join();
				stopLogicThread();

				sock_.close();
			}

			void Client::SetParser(common::Parser::parser_ptr parser)
			{
				parser_ = parser;
			}

			void Client::Handle(int msgID, handler h)
			{
				handler_[msgID] = h;
			}

			void Client::EnableHeartbeat(unsigned long milliseconds)
			{
				hearbeatInterval_ = milliseconds;
			}

			void Client::Connect(const char* ip, short port)
			{
				endpoint_  = endpoint(boost::asio::ip::address::from_string(ip), port);
				doConnect();

				IOThread_ = std::thread([&]()
				{
					ios_.run();
				});
			}

			void Client::Connect(const wchar_t* ip, short port)
			{

			}

			void Client::WriteMessage(const void* date, int len)
			{
				lock_guard locker(writeLocker_);

				bool isWriting = !writeBufferCache_.empty();

				common::shared_buffer buffer;
				parser_->Marshal(static_cast<const common::byte*>(date), len, buffer);
				writeBufferCache_.push(buffer);

				if (!isWriting)
				{
					doWriteMessage();
				}
			}

			void Client::doConnect()
			{
				sock_.async_connect(endpoint_, [&](const boost::system::error_code& ec)
				{
					if (!ec)
					{
						lock_guard locker(msgLocker_);
						messages_.push(Message(ec, common::DefMsgID_Connect, common::shared_buffer()));

						sock_.async_read_some(buffer_.asio_buffer(), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));

						startLogicThread();
					}
					else
					{
						messages_.push(Message(ec, common::DefMsgID_DisConnect, common::shared_buffer()));
						doReconnect();
					}
				});
			}

			void Client::doReconnect()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(common::AutoReconnectInterval));
				stopLogicThread();
				doConnect();
			}

			void Client::doWriteMessage()
			{
				boost::asio::async_write(sock_, writeBufferCache_.front().asio_buffer(), [&](const boost::system::error_code& ec, std::size_t size)
				{
					using namespace std::chrono;

					lock_guard locker(writeLocker_);

					if (ec)
					{
						// clear writeBufferCache_
						std::queue<common::shared_buffer> empty;
						swap(empty, writeBufferCache_);
					}
					else
					{
						lastWriteTime_ = time_point_cast<seconds>(system_clock::now());

						writeBufferCache_.pop();

						if (!writeBufferCache_.empty())
						{
							doWriteMessage();
						}
					}
				});
			}

			void Client::onRead(const boost::system::error_code& err, size_t len)
			{
				if (!err)
				{
					// 读取完整的包头
					buffer_.resize(buffer_.size() + len);
					if (buffer_.size() < parser_->GetPacketHeadLen())
					{
						sock_.async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
						return;
					}

					// 解析包头
					size_t contentLen = 0;
					bool ok = parser_->Unmarshal(buffer_.get(), buffer_.size(), contentLen);
					if (!ok)
					{
						// 包头解析失败，抛弃所有数据重新接收
						buffer_.clear();
					}
					else
					{
						// 包头解析成功，继续接收正文
						if (buffer_.size() < parser_->GetPacketHeadLen() + contentLen)
						{
							// 读取完整的正文
							sock_.async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
							return;
						}

						// 解析正文
						int msgID = 0;
						common::shared_buffer msg(contentLen);
						bool ok = parser_->Unmarshal(buffer_.get(), buffer_.size(), msgID, msg);
						if (!ok)
						{
							// 正文解析失败，抛弃所有数据重新接收
							buffer_.clear();
						}
						else
						{
							// 包头解析成功，整理数据后继续接收
							if (buffer_.size() > parser_->GetPacketHeadLen() + contentLen)
							{
								size_t remain = buffer_.size() - (parser_->GetPacketHeadLen() + contentLen);
								for (size_t i = 0; i < remain; ++i)
								{
									*(buffer_.get() + i) = *(buffer_.get() + (parser_->GetPacketHeadLen() + contentLen) + i);
								}

								buffer_.resize(remain);
							}
							else
							{
								buffer_.clear();
							}

							lock_guard locker(msgLocker_);
							messages_.push(Message(err, msgID, msg));
						}
					}

					sock_.async_read_some(buffer_.asio_buffer(), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					msgLocker_.lock();
					messages_.push(Message(err, common::DefMsgID_DisConnect, common::shared_buffer()));
					msgLocker_.unlock();
					sock_.close();

					// 重连
					doReconnect();
				}
			}

			void Client::hearbeat()
			{
				using namespace std::chrono;

				if (hearbeatInterval_ != 0)
				{
					time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());
					if ((now - lastWriteTime_).count() >= hearbeatInterval_)
					{
						WriteMessage(nullptr, 0);
					}
				}
			}

			void Client::startLogicThread()
			{
				isWorking_ = true;
				logicThread_ = std::thread([&]()
				{
					using namespace std::chrono;

					while (isWorking_)
					{
						// 分发消息
						Message msg;
						bool hasMsg = false;
						msgLocker_.lock();
						if (!messages_.empty())
						{
							hasMsg = true;
							msg = messages_.front();
							messages_.pop();
						}
						msgLocker_.unlock();

						if (hasMsg)
						{
							auto iter = handler_.find(msg.msgID);
							if (iter != handler_.end())
							{
								iter->second(msg.error, msg.buffer.get(), msg.buffer.size());
							}
						}
						else
						{
							std::this_thread::sleep_for(milliseconds(1));
						}

						// 心跳
						hearbeat();
					}
				});
			}

			void Client::stopLogicThread()
			{
				// clear messages_
				msgLocker_.lock();
				std::queue<Message> empty;
				messages_.swap(empty);
				msgLocker_.unlock();

				isWorking_ = false;
				logicThread_.join();
			}

		}// namespace client
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_CLIENT_DXG_HPP
