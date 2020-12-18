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
				enum UserDataIndex : int
				{
					UserDataIndex_0 = 0,
					UserDataIndex_1,
					UserDataIndex_2,
					UserDataIndex_3,
					UserDataIndex_4,
					UserDataIndex_5,
					UserDataIndex_6,
					UserDataIndex_7,
					UserDataIndex_8,
					UserDataIndex_9,
					UserDataIndex_End
				};
			public:
				// ������Ϣ������
				void SetParser(Parser::ptr parser);

				// �����Զ�������
				void SetUserData(const char* key, boost::any data);
				// �����Զ�������(������)
				void SetUserData(UserDataIndex index, boost::any data);

				// ��ȡ�Զ�������
				template<class T>
				bool GetUserData(const char* key, T& data);
				// ��ȡ�Զ�������(������)
				template<class T>
				bool GetUserData(UserDataIndex index, T& data);

				// ɾ��ָ�����Զ�������
				void DeleteUserData(const char* key);
				// ɾ��ָ�����Զ�������(������)
				void DeleteUserData(UserDataIndex index);

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
				void initSocket(socket_ptr sock);
				void postRead();
				socket_ptr getSocket();
			private:
				void onRead(const boost::system::error_code& err, size_t len);
				void doWriteMessage(const common::shared_buffer msg);
				unsigned int hashcode(const char* str) const;
			private:
				socket_ptr sock_;
				std::map<unsigned int, boost::any> userData_;
				boost::any userData2_[UserDataIndex_End];
				std::mutex userDataLocker_;
				Parser::ptr parser_;
				std::mutex writeLocker_;
				std::mutex closeLocker_;
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
				, buffer_(1024 * 16)
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

			inline void BasicSession::SetUserData(const char* key, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData_[hashcode(key)] = data;
			}

			void BasicSession::SetUserData(UserDataIndex index, boost::any data)
			{
				lock_guard locker(userDataLocker_);
				
				userData2_[index] = data;
			}

			template<class T>
			inline bool BasicSession::GetUserData(const char* key, T& data)
			{
				lock_guard locker(userDataLocker_);

				auto iter = userData_.find(hashcode(key));
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

			template<class T>
			bool BasicSession::GetUserData(UserDataIndex index, T& data)
			{
				lock_guard locker(userDataLocker_);

				try
				{
					data = boost::any_cast<T>(userData2_[index]);
				}
				catch (...)
				{
					return false;
				}

				return true;
			}

			inline void BasicSession::DeleteUserData(const char* key)
			{
				lock_guard locker(userDataLocker_);

				userData_.erase(hashcode(key));
			}

			void BasicSession::DeleteUserData(UserDataIndex index)
			{
				lock_guard locker(userDataLocker_);
				
				userData2_[index] = boost::any();
			}

			inline void BasicSession::DeleteAllUserData()
			{
				lock_guard locker(userDataLocker_);

				userData_.clear();

				auto any = boost::any();
				for (int i = 0; i < UserDataIndex_End; ++i)
				{
					userData2_[i] = any;
				}
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
				shared_buffer buffer;
				if (parser_)
				{
					parser_->Marshal(this, static_cast<const unsigned char*>(date), len, buffer);
					buffer.reserve(buffer.size());
				}
				else
				{
					buffer.reserve(len);
					memcpy(buffer.get(), date, len);
				}
				
				writeLocker_.lock();
				++sendPacketCount_;
				if (sendPacketCount_ == 0) ++sendPacketCount_;
				writeLocker_.unlock();

				lock_guard locker(writeLocker_);
				bool isWriting = !writeBufferCache_.empty();
				writeBufferCache_.push(buffer);
				if (!isWriting)
				{
					doWriteMessage(writeBufferCache_.front());
				}
			}

			inline void BasicSession::Close()
			{
				lock_guard locker(closeLocker_);

				if (sock_->is_open())
				{
					sock_->close();
				}

				sendPacketCount_ = 0;
				recvPacketCount_ = 0;
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

				if (err)
				{
					lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
					onPacket(err, common::DefMsgID_DisConnect, common::shared_buffer());
					buffer_.clear();
					return;
				}

				buffer_.resize(buffer_.size() + len);

				enum DataError
				{
					DataError_Uncomplete,			// ��ͷ������
					DataError_ParseFail				// ���ĸ�ʽ����ȷ
				};

				try
				{
					while (!buffer_.empty())
					{
						if (parser_)
						{

							// ��ȡ�����İ�ͷ
							if (buffer_.size() < parser_->GetPacketHeadLen()) throw DataError_Uncomplete;

							// ������ͷ
							size_t contentLen = 0;
							if (!parser_->UnmarshalHead(this, buffer_.get(), buffer_.size(), contentLen)) throw DataError_ParseFail;

							// ��ͷ�����ɹ���������������
							if (buffer_.size() < parser_->GetPacketHeadLen() + contentLen) throw DataError_Uncomplete;

							// ��������
							int msgID = 0;
							common::shared_buffer msg;
							if (!parser_->UnmarshalContent(this, buffer_.get(), buffer_.size(), msgID, msg)) throw DataError_ParseFail;

							// ��ͷ�����ɹ�
							lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
							onPacket(err, msgID, msg);

							++recvPacketCount_;
							if (recvPacketCount_ == 0) ++recvPacketCount_;

							// �������ݺ��������
							if (buffer_.size() > parser_->GetPacketHeadLen() + contentLen)
							{
								size_t remain = buffer_.size() - (parser_->GetPacketHeadLen() + contentLen);
								memmove(buffer_.get(), buffer_.get() + parser_->GetPacketHeadLen() + contentLen, remain);
								buffer_.resize(remain);
							}
							else
							{
								buffer_.clear();
							}
						}
						else
						{
							buffer_.clear();
						}
					}

					sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
				}
				catch (DataError stat)
				{
					switch (stat)
					{
					case DataError_Uncomplete:
						// �������������ı���
						sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						break;
					case DataError_ParseFail:
						// ���������������½���
						buffer_.clear();
						sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						break;
					default:
						break;
					}
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

			unsigned int BasicSession::hashcode(const char* str) const
			{
				unsigned int seed = 131;
				unsigned int hash = 0;
				while (*str)
				{
					hash = hash * seed + (*str++);
				}

				return (hash & 0x7fffffff);
			}

		}// namespace common
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_COMMON_BASIC_SESSION_HPP

