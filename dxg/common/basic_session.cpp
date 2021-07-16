#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include <boost/format.hpp>
#include "basic_session.h"
#include "parser.h"
#include "define.h"

namespace daxia
{
	namespace dxg
	{
		namespace common
		{
			BasicSession::BasicSession()
				: sendPacketCount_(0)
				, recvPacketCount_(0)
				, buffer_(1024 * 16)
			{
			}

			BasicSession::~BasicSession()
			{
				Close();
			}

			void BasicSession::SetParser(std::shared_ptr<Parser> parser)
			{
				parser_ = parser;
			}

			void BasicSession::SetUserData(const char* key, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData_[hashcode(key)] = data;
			}

			void BasicSession::SetUserData(UserDataIndex index, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData2_[index] = data;
			}

			void BasicSession::DeleteUserData(const char* key)
			{
				lock_guard locker(userDataLocker_);

				userData_.erase(hashcode(key));
			}

			void BasicSession::DeleteUserData(UserDataIndex index)
			{
				lock_guard locker(userDataLocker_);

				userData2_[index] = boost::any();
			}

			void BasicSession::DeleteAllUserData()
			{
				lock_guard locker(userDataLocker_);

				userData_.clear();

				auto any = boost::any();
				for (int i = 0; i < UserDataIndex_End; ++i)
				{
					userData2_[i] = any;
				}
			}

			std::string BasicSession::GetPeerAddr() const
			{
				endpoint ep = sock_->remote_endpoint();
				return (boost::format("%s:%d") % ep.address().to_string() % ep.port()).str();
			}

			const daxia::dxg::common::BasicSession::timepoint& BasicSession::GetConnectTime() const
			{
				return connectTime_;
			}

			const daxia::dxg::common::BasicSession::timepoint& BasicSession::UpdateConnectTime()
			{
				using namespace std::chrono;
				connectTime_ = time_point_cast<milliseconds>(system_clock::now());

				return connectTime_;
			}

			const BasicSession::timepoint& BasicSession::GetLastReadTime() const
			{
				return lastReadTime_;
			}

			const BasicSession::timepoint& BasicSession::GetLastWriteTime() const
			{
				return lastWriteTime_;
			}

			unsigned long long BasicSession::GetSendPacketCount() const
			{
				return sendPacketCount_;
			}

			unsigned long long BasicSession::GetRecvPacketCount() const
			{
				return recvPacketCount_;
			}

			void BasicSession::WriteMessage(const void* data, int len)
			{
				shared_buffer buffer;
				if (parser_)
				{
					parser_->Marshal(this, static_cast<const unsigned char*>(data), len, buffer);
					buffer.reserve(buffer.size());
				}
				else
				{
					buffer.reserve(len);
					memcpy(buffer.get(), data, len);
				}

				lock_guard locker(writeLocker_);
				++sendPacketCount_;
				if (sendPacketCount_ == 0) ++sendPacketCount_;
				bool isWriting = !writeBufferCache_.empty();
				writeBufferCache_.push(buffer);
				if (!isWriting)
				{
					doWriteMessage(writeBufferCache_.front());
				}
			}

			void BasicSession::WriteMessage(const std::string& data)
			{
				WriteMessage(data.c_str(), data.size());
			}

			void BasicSession::Close()
			{
				lock_guard locker(closeLocker_);

				if (sock_ && sock_->is_open())
				{
					sock_->close();
				}

				sendPacketCount_ = 0;
				recvPacketCount_ = 0;
			}

			void BasicSession::initSocket(socket_ptr sock)
			{
				sock_ = sock;
			}

			void BasicSession::postRead()
			{
				sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
			}

			BasicSession::socket_ptr BasicSession::getSocket()
			{
				return sock_;
			}

			void BasicSession::onRead(const boost::system::error_code& err, size_t len)
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
					DataError_Uncomplete,			// 包头不完整
					DataError_ParseFail				// 报文格式不正确
				};

				try
				{
					while (!buffer_.empty())
					{
						if (parser_)
						{

							// 读取完整的包头
							if (buffer_.size() < parser_->GetPacketHeadLen()) throw DataError_Uncomplete;

							// 解析包头
							size_t contentLen = 0;
							if (!parser_->UnmarshalHead(this, buffer_.get(), static_cast<int>(buffer_.size()), contentLen)) throw DataError_ParseFail;

							if (parser_->GetPacketHeadLen() + contentLen > buffer_.capacity())
							{
								buffer_.reserve(parser_->GetPacketHeadLen() + contentLen);
							}

							// 包头解析成功，继续接收正文
							if (buffer_.size() < parser_->GetPacketHeadLen() + contentLen) throw DataError_Uncomplete;

							// 解析正文
							int msgID = 0;
							common::shared_buffer msg;
							if (!parser_->UnmarshalContent(this, buffer_.get(), static_cast<int>(buffer_.size()), msgID, msg)) throw DataError_ParseFail;

							// 包头解析成功
							lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
							onPacket(err, msgID, msg);

							++recvPacketCount_;
							if (recvPacketCount_ == 0) ++recvPacketCount_;

							// 整理数据后继续接收
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
						// 继续接收完整的报文
						sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						break;
					case DataError_ParseFail:
						// 抛弃所有数据重新接收
						buffer_.clear();
						sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
						break;
					default:
						break;
					}
				}
			}

			void BasicSession::doWriteMessage(const common::shared_buffer msg)
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

