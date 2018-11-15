/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file dxg.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮ�� 2018
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
#include <daxia/encode/strconv.hpp>

#define DXG_CLIENT_HANDLER(id,error,date,len) [&](int id, const boost::system::error_code& error, const void* date, int len)

namespace daxia
{
	namespace dxg
	{
		namespace client
		{
			// �ͻ�����
			class Client
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
			public:
				void SetParser(common::Parser::parser_ptr parser);
				void Handle(int msgID, handler h);
				void EnableHeartbeat(unsigned long milliseconds);
				void Connect(const char* ip, short port);
				void Connect(const wchar_t* ip, short port);
				void Close();
				void WriteMessage(const void* date, int len);
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
				void doWriteMessage();
				void onRead(const boost::system::error_code& err, size_t len);
				void hearbeat();
				void startLogicThread();
				void stopLogicThread();
				void clearMessage();
				void pushLogciMessage(const LogicMessage& msg);
			private:
				boost::asio::io_service netIoService_;
				boost::asio::io_service logicIoService_;
				socket sock_;
				common::shared_buffer buffer_;
				std::thread ioThread_;
				std::thread logicThread_;
				std::shared_ptr<common::Parser> parser_;
				std::map<int, handler> handler_;
				std::mutex writeLocker_;
				std::queue<common::shared_buffer> writeBufferCache_;
				bool isIoWorking_;
				bool isLogicWorking_;
				std::queue<LogicMessage> logicMsgs_;
				std::mutex logicMsgLocker_;
				timepoint lastWriteTime_;
				unsigned long hearbeatInterval_;
				endpoint endpoint_;
			};

			inline Client::Client()
				: sock_(netIoService_)
				, buffer_(1024 * 8)
				, isIoWorking_(false)
				, isLogicWorking_(false)
				, hearbeatInterval_(0)
			{
				parser_ = common::Parser::parser_ptr(new common::DefaultParser);
				startLogicThread();
			}

			inline Client::~Client()
			{
				stopLogicThread();
				Close();
			}

			inline void Client::SetParser(common::Parser::parser_ptr parser)
			{
				parser_ = parser;
			}

			inline void Client::Handle(int msgID, handler h)
			{
				handler_[msgID] = h;
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
						// ����netIoService_.run���˳�
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

			inline void Client::Close()
			{
				isIoWorking_ = false;
				netIoService_.stop();
				if (ioThread_.joinable())
				{
					ioThread_.join();
				}

				sock_.close();
				clearMessage();
			}

			inline void Client::WriteMessage(const void* date, int len)
			{
				lock_guard locker(writeLocker_);

				bool isWriting = !writeBufferCache_.empty();

				common::shared_buffer buffer;
				parser_->Marshal(this, static_cast<const unsigned char*>(date), len, buffer);
				writeBufferCache_.push(buffer);

				if (!isWriting)
				{
					doWriteMessage();
				}
			}

			inline void Client::doConnect()
			{
				sock_.async_connect(endpoint_, [&](const boost::system::error_code& ec)
				{
					pushLogciMessage(LogicMessage(ec, common::DefMsgID_Connect, common::shared_buffer()));

					if (!ec)
					{
						sock_.async_read_some(buffer_.asio_buffer(), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
					}
				});
			}

			inline void Client::doWriteMessage()
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

			inline void Client::onRead(const boost::system::error_code& err, size_t len)
			{
				if (!err)
				{
					// ��ȡ�����İ�ͷ
					buffer_.resize(buffer_.size() + len);
					if (buffer_.size() < parser_->GetPacketHeadLen())
					{
						sock_.async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
						return;
					}

					// ������ͷ
					size_t contentLen = 0;
					bool ok = parser_->Unmarshal(this, buffer_.get(), buffer_.size(), contentLen);
					if (!ok)
					{
						// ��ͷ����ʧ�ܣ����������������½���
						buffer_.clear();
					}
					else
					{
						// ��ͷ�����ɹ���������������
						if (buffer_.size() < parser_->GetPacketHeadLen() + contentLen)
						{
							// ��ȡ����������
							sock_.async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
							return;
						}

						// ��������
						int msgID = 0;
						common::shared_buffer msg(contentLen);
						bool ok = parser_->Unmarshal(this, buffer_.get(), buffer_.size(), msgID, msg);
						if (!ok)
						{
							// ���Ľ���ʧ�ܣ����������������½���
							buffer_.clear();
						}
						else
						{
							// ��ͷ�����ɹ����������ݺ��������
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

							pushLogciMessage(LogicMessage(err, msgID, msg));
						}
					}

					sock_.async_read_some(buffer_.asio_buffer(), std::bind(&Client::onRead, this, std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					pushLogciMessage(LogicMessage(err, common::DefMsgID_DisConnect, common::shared_buffer()));
				}
			}

			inline void Client::hearbeat()
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

			inline void Client::startLogicThread()
			{
				isLogicWorking_ = true;
				logicThread_ = std::thread([&]()
				{
					boost::asio::deadline_timer timer(logicIoService_, boost::posix_time::milliseconds(1000));
					while (isLogicWorking_)
					{
						// ����logicIoService_.run���˳�
						timer.async_wait([&](const boost::system::error_code& ec)
						{
							// ����
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
					// �ַ���Ϣ
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
