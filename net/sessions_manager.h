/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file sessions_manager.h
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_NET_SERVER_SESSIONS_MANAGER_H
#define __DAXIA_NET_SERVER_SESSIONS_MANAGER_H

#include <unordered_map>
#include "session.h"
#include "scheduler.h"

namespace daxia
{
	namespace net
	{
		// �Ự������
		class SessionsManager
		{
		public:
			typedef std::lock_guard<std::mutex> lock_guard;
			typedef std::shared_ptr<SessionsManager> ptr;
		public:
			SessionsManager();
			virtual ~SessionsManager(){}
		public:
			// ����һ���ͻ���
			void AddSession(Session::ptr session);
			// ɾ��һ���ͻ���
			void DeleteSession(long long id);
			// ɾ�����пͻ���
			void DeleteAllSession();
			// ��ȡ�ͻ���
			Session::ptr GetSession(long long id);
			// �����Զ������ݻ�ȡ�ͻ���
			template<typename T>
			Session::ptr GetSession(const daxia::string& key, const T& data);
			// ��һ���ͻ�����㲥һ����Ϣ
			SessionsManager::ptr Broadcast(int msgId, const daxia::string& name, const daxia::string& msg, const common::PageInfo* pageInfo = nullptr, size_t maxPacketLength = common::MaxBufferSize);
			// ����һ���ͻ�����
			SessionsManager::ptr CreateGroup(const daxia::string& name);
			// ɾ��һ���ͻ�����
			void DeleteGroup(const daxia::string& name);
			// ɾ�����пͻ�����
			void DeleteAllGroup();
			// ��ȡ�ͻ�����
			SessionsManager::ptr GetGroup(const daxia::string& name);
			// ö�ٿͻ���
			void EnumSession(std::function<bool(Session::ptr)> func);
			// ö�ٿͻ�����
			void EnumGroup(std::function<bool(SessionsManager::ptr)> func);
			// ��ȡ�ͻ�������
			size_t SessionSize() const;
		private:
			std::unordered_map<long long, Session::ptr> sessions_;
			std::unordered_map<daxia::string, SessionsManager::ptr> group_;
			std::mutex sessionsLocker_;
			std::mutex groupLocker_;
		};

		template<typename T>
		inline std::shared_ptr<Session> SessionsManager::GetSession(const daxia::string& key, const T& data)
		{
			std::shared_ptr<Session> session;

			lock_guard locker(sessionsLocker_);

			for (auto clientIter = sessions_.begin();
				clientIter != sessions_.end();
				++clientIter)
			{
				T* userdata = clientIter->second->GetUserData<T>(key.GetString());
				if (userdata != nullptr)
				{
					if (*userdata == data)
					{
						session = clientIter->second;
						break;
					}
				}
			}

			return session;
		}
	}// namespace net
}// namespace daxia

#endif // !__DAXIA_NET_SERVER_SESSIONS_MANAGER_H
