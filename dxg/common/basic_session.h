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

#ifndef __DAXIA_COMMON_BASIC_SESSION_H
#define __DAXIA_COMMON_BASIC_SESSION_H

#include <mutex>
#include <memory>
#include <queue>
#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "shared_buffer.h"

namespace daxia
{
	namespace dxg
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
				template<class T>
				bool GetUserData(const char* key, T& data);
				// 获取自定义数据(高性能)
				template<class T>
				bool GetUserData(UserDataIndex index, T& data);

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
				void WriteMessage(const void* data, int len);
				void WriteMessage(const std::string& data);

				// 关闭会话
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
				std::shared_ptr<Parser> parser_;
				std::mutex writeLocker_;
				std::mutex closeLocker_;
				std::queue<shared_buffer> writeBufferCache_;
				timepoint connectTime_;
				timepoint lastReadTime_;
				timepoint lastWriteTime_;
				unsigned long long sendPacketCount_;
				unsigned long long recvPacketCount_;
				common::shared_buffer buffer_;
			};

			template<class T>
			bool BasicSession::GetUserData(const char* key, T& data)
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

		}// namespace common
	}// namespace dxg
}// namespace daxia
#endif // !__DAXIA_COMMON_BASIC_SESSION_H

