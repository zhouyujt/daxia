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
				, buffer_(MaxBufferSize)
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
				WriteMessage(data.c_str(), static_cast<int>(data.size()));
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

				Parser::Result result = Parser::Result::Result_Success;
				while (!buffer_.empty())
				{
					if (parser_)
					{
						int msgID = 0;
						int packetLen = 0;
						common::shared_buffer msg;
						Parser::Result result = parser_->Unmarshal(this, buffer_.get(), static_cast<int>(buffer_.size()), msgID, msg, packetLen);

						if (result != Parser::Result::Result_Success)
						{
							break;
						}

						// 包头解析成功
						lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
						onPacket(err, msgID, msg);

						++recvPacketCount_;
						if (recvPacketCount_ == 0) ++recvPacketCount_;

						// 整理数据后继续接收
						if (buffer_.size() > packetLen)
						{
							size_t remain = buffer_.size() - packetLen;
							memmove(buffer_.get(), buffer_.get() + packetLen, remain);
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

				// 不允许超过限定的缓冲区最大值，服务器不缓存过多数据，交由通讯双方自行拆包处理
				if (buffer_.size() >= MaxBufferSize)
				{
					result = Parser::Result::Result_Fail;
				}

				switch (result)
				{
				case Parser::Result::Result_Success:
					sock_->async_read_some(buffer_.asio_buffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
					break;
				case Parser::Result::Result_Fail:
					// 断开连接
					buffer_.clear();
					Close();
					onPacket(err, common::DefMsgID_DisConnect, common::shared_buffer());
					break;
				case Parser::Result::Result_Uncomplete:
					// 继续接收完整的报文
					sock_->async_read_some(buffer_.asio_buffer(buffer_.size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
					break;
				default:
					break;
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
