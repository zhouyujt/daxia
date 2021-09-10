#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <boost/format.hpp>
#include "basic_session.h"
#include "parser.h"
#include "define.h"

namespace daxia
{
	namespace net
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

				// 等待写线程处理完毕
				lock_guard locker(writeLocker_);
			}

			void BasicSession::SetParser(std::shared_ptr<Parser> parser)
			{
				parser_ = parser;
			}

			void BasicSession::SetUserData(const char* key, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData_[static_cast<int>(daxia::string(key).Hash())] = data;
			}

			void BasicSession::SetUserData(UserDataIndex index, boost::any data)
			{
				lock_guard locker(userDataLocker_);

				userData2_[index] = data;
			}

			void BasicSession::DeleteUserData(const char* key)
			{
				lock_guard locker(userDataLocker_);

				userData_.erase(static_cast<int>(daxia::string(key).Hash()));
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
				endpoint ep;

				try
				{
					ep = sock_->remote_endpoint();
				}
				catch (...)
				{
					return "";
				}

				return (boost::format("%s:%d") % ep.address().to_string() % ep.port()).str();
			}

			const daxia::net::common::BasicSession::timepoint& BasicSession::GetConnectTime() const
			{
				return connectTime_;
			}

			const daxia::net::common::BasicSession::timepoint& BasicSession::UpdateConnectTime()
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

			void BasicSession::WriteMessage(int msgId, const void* data, size_t len, const common::PageInfo* pageInfo, size_t maxPacketLength)
			{
				std::vector<Buffer> buffers;
				if (parser_)
				{
					parser_->Marshal(this, msgId, data, len, pageInfo, buffers, maxPacketLength);

					lock_guard locker(writeLocker_);
					bool isWriting = !writeBufferCache_.empty();

					if (!buffers.empty())
					{
						for (const Buffer& buffer : buffers)
						{
							writeBufferCache_.push(buffer);
							++sendPacketCount_;
							if (sendPacketCount_ == 0) ++sendPacketCount_;
						}

						if (!isWriting)
						{
							doWriteMessage(writeBufferCache_.front());
						}
					}
				}
			}

			void BasicSession::WriteMessage(int msgId, const std::string& data, const common::PageInfo* pageInfo, size_t maxPacketLength)
			{
				WriteMessage(msgId, data.c_str(), data.size(), pageInfo, maxPacketLength);
			}

			void BasicSession::WriteMessage(int msgId, const daxia::string& data, const common::PageInfo* pageInfo, size_t maxPacketLength)
			{
				WriteMessage(msgId, data.GetString(), data.GetLength(), pageInfo, maxPacketLength);
			}

			void BasicSession::WriteRawData(const void* data, size_t len)
			{
				Buffer buffer;
				buffer.Reserve(len);
				memcpy(buffer, data, len);

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

			void BasicSession::WriteRawData(const std::string& data)
			{
				WriteRawData(data.c_str(), data.size());
			}

			void BasicSession::WriteRawData(const daxia::string& data)
			{
				WriteRawData(data.GetString(), data.GetLength());
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
				sock_->async_read_some(buffer_.GetAsioBuffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
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
					onPacket(err, common::DefMsgID_DisConnect, common::Buffer());
					buffer_.Clear();
					return;
				}

				buffer_.Resize(buffer_.Size() + len);

				Parser::Result result = Parser::Result::Result_Success;
				while (!buffer_.IsEmpty())
				{
					if (parser_)
					{
						int msgID = 0;
						size_t packetLen = 0;
						common::Buffer msg;
						result = parser_->Unmarshal(this, buffer_, buffer_.Size(), msgID, msg, packetLen);

						if (result != Parser::Result::Result_Success)
						{
							break;
						}

						// 包头解析成功
						lastReadTime_ = time_point_cast<milliseconds>(system_clock::now());
						onPacket(err, msgID, msg);

						if (msg.Page().IsEnd())
						{
							++recvPacketCount_;
							if (recvPacketCount_ == 0) ++recvPacketCount_;
						}

						// 整理数据后继续接收
						if (buffer_.Size() > packetLen)
						{
							size_t remain = buffer_.Size() - packetLen;
							memmove(buffer_, buffer_ + packetLen, remain);
							buffer_.Resize(remain);
						}
						else
						{
							buffer_.Clear();
						}
					}
					else
					{
						buffer_.Clear();
					}
				}

				// 不允许超过限定的缓冲区最大值，服务器不缓存过多数据，交由通讯双方自行拆包处理
				if (buffer_.Size() >= MaxBufferSize)
				{
					result = Parser::Result::Result_Fail;
				}

				switch (result)
				{
				case Parser::Result::Result_Success:
					sock_->async_read_some(buffer_.GetAsioBuffer(), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
					break;
				case Parser::Result::Result_Fail:
					// 断开连接
					buffer_.Clear();
					Close();
					onPacket(err, common::DefMsgID_DisConnect, common::Buffer());
					break;
				case Parser::Result::Result_Uncomplete:
					// 继续接收完整的报文
					sock_->async_read_some(buffer_.GetAsioBuffer(buffer_.Size()), std::bind(&BasicSession::onRead, this, std::placeholders::_1, std::placeholders::_2));
					break;
				default:
					break;
				}
			}

			void BasicSession::doWriteMessage(const common::Buffer& msg)
			{
				using namespace std::chrono;
				boost::asio::async_write(*sock_, msg.GetAsioBuffer(), [&](const boost::system::error_code& ec, std::size_t size)
				{
					lock_guard locker(writeLocker_);

					if (ec)
					{
						// clear writeBufferCache_
						std::queue<Buffer> empty;
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
	}// namespace net
}// namespace daxia
