/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file basic_session.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮһ�� 2018
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
				// ������Ϣ������
				void SetParser(Parser::ptr parser);

				// �����Զ�������
				void SetUserData(const std::string& key, boost::any data);

				// ��ȡ�Զ�������
				template<class T>
				bool GetUserData(const std::string& key, T& data);

				// ɾ��ָ�����Զ�������
				void DeleteUserData(const std::string& key);

				// ɾ�������Զ�������
				void DeleteAllUserData();

				// ��ȡԶ�˵�ַ
				std::string GetPeerAddr() const;

				// �������ʱ���
				const timepoint& GetLastReadTime() const;

				// ���д��ʱ���
				const timepoint& GetLastWriteTime() const;

				// ��ȡ���͵����ݰ�����
				unsigned long long GetSendPacketCount() const;

				// ��ȡ���յ������ݰ�����
				unsigned long long GetRecvPacketCount() const;

				// ������Ϣ
				void WriteMessage(const void* date, int len);

				// �رջỰ
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
					// ��ȡ�����İ�ͷ
					buffer_.resize(buffer_.size() + len);
					if (buffer_.size() < parser_->GetPacketHeadLen())
					{
						sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						return;
					}

					// ������ͷ
					size_t contentLen = 0;
					bool ok = parser_->UnmarshalHead(this, buffer_.get(), buffer_.size(), contentLen);
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
							sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
							return;
						}

						// ������
						if (contentLen == 0)
						{
							lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
							onPacket(err, common::DefMsgID_Heartbeat, common::shared_buffer());

							sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
							++recvPacketCount_;
							if (recvPacketCount_ == 0) ++recvPacketCount_;

							return;
						}

						// ��������
						int msgID = 0;
						common::shared_buffer msg(contentLen);
						bool ok = parser_->UnmarshalContent(this, buffer_.get(), buffer_.size(), msgID, msg);
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

