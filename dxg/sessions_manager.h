/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file sessions_manager.hpp
 * \author �콭��Ĵ�Ϻ
 * \date ���� 2018
 *
 */

#ifndef __DAXIA_DXG_SERVER_SESSIONS_MANAGER_H
#define __DAXIA_DXG_SERVER_SESSIONS_MANAGER_H

#include "session.h"
#include "scheduler.h"

namespace daxia
{
	namespace dxg
	{
		// �Ự������
		class SessionsManager
		{
		public:
			typedef std::lock_guard<std::mutex> lock_guard;
		public:
			SessionsManager(Scheduler& scheduler);
			~SessionsManager(){}
		public:
			// ����һ���ͻ���
			std::shared_ptr<Session> AddSession(common::BasicSession::socket_ptr sock, std::shared_ptr<common::Parser> parser, Session::handler onMessage);

			// ɾ��һ���ͻ���
			void DeleteSession(long long id);

			// ��ȡ�ͻ���
			Session::ptr GetSession(long long id);

			// �����Զ������ݻ�ȡ�ͻ���
			template<class T>
			Session::ptr GetSession(const std::string& key, const T& data);

			// �㲥һ����Ϣ
			void Broadcast();

			// ����һ���ͻ�����
			void CreateGroup(const std::string& key);

			// ��ȡ�ͻ�����
			std::shared_ptr<SessionsManager> GetGroup(const std::string& key);

			// ����/�ر��������
			void EnableCheckHeartbeat(unsigned long interval);
		private:
			std::map<long long, Session::ptr> sessions_;
			std::map<std::string, std::shared_ptr<SessionsManager>> group_;
			std::mutex sessionsLocker_;
			std::mutex groupLocker_;
			Scheduler& scheduler_;
			long long heartbeatSchedulerId_;
			long long nextSessionId_;
		};

		template<class T>
		inline std::shared_ptr<Session> SessionsManager::GetSession(const std::string& key, const T& data)
		{
			std::shared_ptr<Session> session;

			lock_guard locker(sessionsLocker_);

			for (auto clientIter = sessions_.begin();
				clientIter != sessions_.end();
				++clientIter)
			{
				auto userDataIter = clientIter->second->userData_.find(key);
				if (userDataIter != clientIter->second->userData_.end() && userDataIter->second.type() == typeid(data))
				{
					try
					{
						if (boost::any_cast<T>(userDataIter->second) == data) session = clientIter->second;
					}
					catch (...)
					{
					}

					if (session) break;
				}
			}

			return session;
		}
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_SERVER_SESSIONS_MANAGER_H
