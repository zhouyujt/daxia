/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file basic_session.h
 * \author �콭��Ĵ�Ϻ
 * \date ʮһ�� 2018
 *
 */

#ifndef __DAXIA_NET_COMMON_BASIC_SESSION_H
#define __DAXIA_NET_COMMON_BASIC_SESSION_H

#include <mutex>
#include <memory>
#include <queue>
#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include "shared_buffer.h"
#include "../../string.hpp"

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
				void SetParser(std::shared_ptr<Parser> parser);

				// �����Զ�������
				void SetUserData(const char* key, boost::any data);
				// �����Զ�������(������)
				void SetUserData(UserDataIndex index, boost::any data);

				// ��ȡ�Զ�������
				template<class T>
				T* GetUserData(const char* key);
				// ��ȡ�Զ�������(������)
				template<class T>
				T* GetUserData(UserDataIndex index);

				// ɾ��ָ�����Զ�������
				void DeleteUserData(const char* key);
				// ɾ��ָ�����Զ�������(������)
				void DeleteUserData(UserDataIndex index);

				// ɾ�������Զ�������
				void DeleteAllUserData();

				// ��ȡԶ�˵�ַ
				std::string GetPeerAddr() const;

				// ����ʱ��
				const timepoint& GetConnectTime() const;

				// ��������ʱ��
				const timepoint& UpdateConnectTime();

				// �������ʱ���
				const timepoint& GetLastReadTime() const;

				// ���д��ʱ���
				const timepoint& GetLastWriteTime() const;

				// ��ȡ���͵����ݰ�����
				unsigned long long GetSendPacketCount() const;

				// ��ȡ���յ������ݰ�����
				unsigned long long GetRecvPacketCount() const;

				// ������Ϣ
				void WriteMessage(const void* data, size_t len);
				void WriteMessage(const std::string& data);
				void WriteMessage(const daxia::string& data);

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
			private:
				socket_ptr sock_;
				std::map<unsigned int, boost::any> userData_;
				boost::any userData2_[UserDataIndex_End + 10/*����ڲ�ʹ��*/];
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

			template<class T>
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

