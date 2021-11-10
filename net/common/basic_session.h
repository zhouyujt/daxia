/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file basic_session.h
 * \author 漓江里的大虾
 * \date 十一月 2018
 *
 */

#ifndef __DAXIA_NET_COMMON_BASIC_SESSION_H
#define __DAXIA_NET_COMMON_BASIC_SESSION_H

#ifdef _WIN32
#include <sdkddkver.h> // boost/asio.hpp需要包含此文件
#endif

#include <mutex>
#include <memory>
#include <queue>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "buffer.h"
#include "../../string.hpp"

#ifdef DAXIA_NET_SUPPORT_HTTPS
#include <boost/asio/ssl.hpp>
#endif

namespace daxia
{
	namespace net
	{
		namespace common
		{
			class Parser;

			class BasicSession
			{
			public:
				typedef std::lock_guard<std::mutex> lock_guard;
				typedef boost::asio::ip::tcp::socket socket;
				typedef std::shared_ptr<socket> socket_ptr;
#ifdef DAXIA_NET_SUPPORT_HTTPS
				typedef boost::asio::ssl::stream<socket> sslsocket;
				typedef std::shared_ptr<sslsocket> sslsocket_ptr;
#endif
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
				// 设置消息解析器
				void SetParser(std::shared_ptr<Parser> parser);

				// 设置自定义数据
				void SetUserData(const char* key, boost::any data);
				// 设置自定义数据(高性能)
				void SetUserData(UserDataIndex index, boost::any data);

				// 获取自定义数据
				template<typename T>
				T* GetUserData(const char* key);
				// 获取自定义数据(高性能)
				template<typename T>
				T* GetUserData(UserDataIndex index);

				// 删除指定的自定义数据
				void DeleteUserData(const char* key);
				// 删除指定的自定义数据(高性能)
				void DeleteUserData(UserDataIndex index);

				// 删除所有自定义数据
				void DeleteAllUserData();

				// 获取远端地址
				std::string GetPeerAddr() const;

				// 连接时间
				const timepoint& GetConnectTime() const;

				// 更新连接时间
				const timepoint& UpdateConnectTime();

				// 最近读的时间戳
				const timepoint& GetLastReadTime() const;

				// 最近写的时间戳
				const timepoint& GetLastWriteTime() const;

				// 获取发送的数据包数量
				unsigned long long GetSendPacketCount() const;

				// 获取接收到的数据包数量
				unsigned long long GetRecvPacketCount() const;

				// 发送消息
				void WriteMessage(int msgId, const void* data, size_t len, const common::PageInfo* pageInfo = nullptr, size_t maxPacketLength = common::MaxBufferSize);
				void WriteMessage(int msgId, const std::string& data, const common::PageInfo* pageInfo = nullptr, size_t maxPacketLength = common::MaxBufferSize);
				void WriteMessage(int msgId, const daxia::string& data, const common::PageInfo* pageInfo = nullptr, size_t maxPacketLength = common::MaxBufferSize);

				// 发送不经过解析器打包的原始数据
				void WriteRawData(const void* data, size_t len);
				void WriteRawData(const std::string& data);
				void WriteRawData(const daxia::string& data);

				// 关闭会话
				void Close();
			protected:
				virtual void onPacket(const boost::system::error_code& error, int msgId, const common::Buffer& buffer) = 0;
				void initSocket(socket_ptr sock);
#ifdef DAXIA_NET_SUPPORT_HTTPS
				void initSocket(sslsocket_ptr sslsock);
#endif
				void postRead();
				socket_ptr getSocket();
#ifdef DAXIA_NET_SUPPORT_HTTPS
				sslsocket_ptr getSSLScoket();
#endif
			private:
				void onRead(const boost::system::error_code& err, size_t len);
				void doWriteMessage(const common::Buffer& msg);
			private:
				socket_ptr sock_;
#ifdef DAXIA_NET_SUPPORT_HTTPS
				sslsocket_ptr sslsock_;
#endif
				std::map<unsigned int, boost::any> userData_;
				boost::any userData2_[UserDataIndex_End + 10/*框架内部使用*/];
				std::mutex userDataLocker_;
				std::shared_ptr<Parser> parser_;
				std::mutex writeLocker_;
				std::mutex closeLocker_;
				std::queue<Buffer> writeBufferCache_;
				timepoint connectTime_;
				timepoint lastReadTime_;
				timepoint lastWriteTime_;
				unsigned long long sendPacketCount_;
				unsigned long long recvPacketCount_;
				common::Buffer buffer_;
			};

			template<typename T>
			T* BasicSession::GetUserData(const char* key)
			{
				lock_guard locker(userDataLocker_);

				T* data = nullptr;

				auto iter = userData_.find(static_cast<int>(daxia::string(key).Hash()));
				if (iter != userData_.end())
				{
					try
					{
						data = boost::any_cast<T>(&iter->second);
					}
					catch (...)
					{
					}
				}

				return  data;
			}

			template<typename T>
			T* BasicSession::GetUserData(UserDataIndex index)
			{
				lock_guard locker(userDataLocker_);

				if (index >= UserDataIndex_End + 10) return nullptr;

				T* data = nullptr;
				try
				{
					data = boost::any_cast<T>(&userData2_[index]);
				}
				catch (...)
				{
				}

				return data;
			}

		}// namespace common
	}// namespace net
}// namespace daxia
#endif // !__DAXIA_NET_COMMON_BASIC_SESSION_H

