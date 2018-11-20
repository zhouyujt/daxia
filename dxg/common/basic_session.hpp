/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file basic_session.hpp
 * \author 漓江里的大虾
 * \date 十一月 2018
 * 
 */

#ifndef __DAXIA_COMMON_BASIC_SESSION_HPP
#define __DAXIA_COMMON_BASIC_SESSION_HPP

#include <mutex>
#include <memory>
#include <queue>
#include <boost/any.hpp>
#include <daxia/dxg/common/define.hpp>
#include <daxia/dxg/common/parser.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			class BasicSession
			{
			public:
				typedef std::lock_guard<std::mutex> lock_guard;
				typedef boost::asio::ip::tcp::socket socket;
				typedef std::shared_ptr<socket> socket_ptr;
				typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timepoint;
				typedef boost::asio::ip::tcp::endpoint endpoint;
			protected:
				BasicSession();
			public:
				virtual ~BasicSession();
			public:
				// 设置消息解析器
				void SetParser(Parser::ptr parser);

				// 设置自定义数据
				void SetUserData(const std::string& key, boost::any data);

				// 获取自定义数据
				template<class T>
				bool GetUserData(const std::string& key, T& data);

				// 删除指定的自定义数据
				void DeleteUserData(const std::string& key);

				// 删除所有自定义数据
				void DeleteAllUserData();

				// 获取远端地址
				std::string GetPeerAddr() const;

				// 最近读的时间戳
				const timepoint& GetLastReadTime() const;

				// 最近写的时间戳
				const timepoint& GetLastWriteTime() const;

				// 获取发送的数据包数量
				unsigned long long GetSendPacketCount() const;

				// 获取接收到的数据包数量
				unsigned long long GetRecvPacketCount() const;

				// 发送消息
				void WriteMessage(const void* date, int len);

				// 关闭会话
				void Close();
			protected:
				virtual void onPacket(const boost::system::error_code& error, int msgId, const common::shared_buffer& buffer) = 0;
				virtual void onClose(){}
				void initSocket(socket_ptr sock);
				void postRead();
				socket_ptr getSocket();
			private:
				void onRead(const boost::system::error_code& err, size_t len);
				void doWriteMessage(const common::shared_buffer msg);
			private:
				socket_ptr sock_;
				std::map<std::string, boost::any> userData_;
				std::mutex userDataLocker_;
				Parser::ptr parser_;
				std::mutex writeLocker_;
				std::queue<shared_buffer> writeBufferCache_;
				timepoint lastReadTime_;
				timepoint lastWriteTime_;
				unsigned long long sendPacketCount_;
				unsigned long long recvPacketCount_;
				common::shared_buffer buffer_;
			};

			inline BasicSession::BasicSession()
				: sendPacketCount_(0)
				, recvPacketCount_(0)
				, buffer_(1024 * 8)
			{

			}

			inline BasicSession::~BasicSession()
			{
				Close();
			}

			inline void BasicSession::SetParser(Parser::ptr parser)
			{
				parser_ = parser;
			}

			inline void BasicSession::SetUserData(const std::string& key, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData_[key] = data;
			}

			template<class T>
			inline bool BasicSession::GetUserData(const std::string& key, T& data)
			{
				lock_guard locker(userDataLocker_);

				auto iter = userData_.find(key);
				if (iter != userData_.end())
				{
					try
					{
						data = boost::any_cast<T>(iter->second);
					}
					catch (...)
					{
						return false;
					}

					return true;
				}

				return  false;
			}

			inline void BasicSession::DeleteUserData(const std::string& key)
			{
				lock_guard locker(userDataLocker_);

				userData_.erase(key);
			}

			inline void BasicSession::DeleteAllUserData()
			{
				lock_guard locker(userDataLocker_);

				userData_.clear();
			}

			inline std::string BasicSession::GetPeerAddr() const
			{
				endpoint ep =  sock_->remote_endpoint();
				return (boost::format("%s:%d") % ep.address().to_string() % ep.port()).str();
			}

			inline const BasicSession::timepoint& BasicSession::GetLastReadTime() const
			{
				return lastReadTime_;
			}

			inline const BasicSession::timepoint& BasicSession::GetLastWriteTime() const
			{
				return lastWriteTime_;
			}

			inline unsigned long long BasicSession::GetSendPacketCount() const
			{
				return sendPacketCount_;
			}

			inline unsigned long long BasicSession::GetRecvPacketCount() const
			{
				return recvPacketCount_;
			}

			inline void BasicSession::WriteMessage(const void* date, int len)
			{
				lock_guard locker(writeLocker_);

				bool isWriting = !writeBufferCache_.empty();

				shared_buffer buffer;
				parser_->Marshal(this, static_cast<const unsigned char*>(date), len, buffer);
				writeBufferCache_.push(buffer);
				++sendPacketCount_;
				if (sendPacketCount_ == 0) ++sendPacketCount_;

				if (!isWriting)
				{
					doWriteMessage(writeBufferCache_.front());
				}
			}

			inline void BasicSession::Close()
			{
				if (sock_) sock_->close();
				onClose();
			}

			inline void BasicSession::initSocket(socket_ptr sock)
			{
				sock_ = sock;
			}

			inline void BasicSession::postRead()
			{
				sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
			}

			inline BasicSession::socket_ptr BasicSession::getSocket()
			{
				return sock_;
			}

			inline void BasicSession::onRead(const boost::system::error_code& err, size_t len)
			{
				using namespace std::chrono;

				if (!err)
				{
					// 读取完整的包头
					buffer_.resize(buffer_.size() + len);
					if (buffer_.size() < parser_->GetPacketHeadLen())
					{
						sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						return;
					}

					// 解析包头
					size_t contentLen = 0;
					bool ok = parser_->UnmarshalHead(this, buffer_.get(), buffer_.size(), contentLen);
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
							sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
							return;
						}

						// 心跳包
						if (contentLen == 0)
						{
							lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
							onPacket(err, common::DefMsgID_Heartbeat, common::shared_buffer());

							sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
							++recvPacketCount_;
							if (recvPacketCount_ == 0) ++recvPacketCount_;

							return;
						}

						// 解析正文
						int msgID = 0;
						common::shared_buffer msg(contentLen);
						bool ok = parser_->UnmarshalContent(this, buffer_.get(), buffer_.size(), msgID, msg);
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

							lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
							onPacket(err, msgID, msg);

							++recvPacketCount_;
						}
					}

					sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
					onPacket(err, common::DefMsgID_DisConnect, common::shared_buffer());
				}
			}

			inline void BasicSession::doWriteMessage(const common::shared_buffer msg)
			{
				using namespace std::chrono;

				boost::asio::async_write(*sock_, msg.asio_buffer(), [&](const boost::system::error_code& ec, std::size_t size)
				{
					lock_guard locker(writeLocker_);

					if (ec)
					{
						// clear writeBufferCache_
						std::queue<shared_buffer> empty;
						swap(empty, writeBufferCache_);
					}
					else
					{
						lastWriteTime_ = time_point_cast<seconds>(system_clock::now());

						writeBufferCache_.pop();

						if (!writeBufferCache_.empty())
						{
							doWriteMessage(writeBufferCache_.front());
						}
					}
				});
			}

		}// namespace common
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_COMMON_BASIC_SESSION_HPP

