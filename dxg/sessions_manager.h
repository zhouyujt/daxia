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
			typedef std::shared_ptr<SessionsManager> ptr;
		public:
			SessionsManager();
			~SessionsManager(){}
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
			template<class T>
			Session::ptr GetSession(const std::string& key, const T& data);
			// ��һ���ͻ�����㲥һ����Ϣ
			void Broadcast(const std::string& name,const std::string& msg);
			// ����һ���ͻ�����
			SessionsManager::ptr CreateGroup(const std::string& name);
			// ɾ��һ���ͻ�����
			void DeleteGroup(const std::string& name);
			// ɾ�����пͻ�����
			void DeleteAllGroup();
			// ��ȡ�ͻ�����
			SessionsManager::ptr GetGroup(const std::string& name);
			// ö�ٿͻ���
			void EnumSession(std::function<bool(Session::ptr)> func);
			// ö�ٿͻ�����
			void EnumGroup(std::function<bool(SessionsManager::ptr)> func);
		private:
			std::map<long long, Session::ptr> sessions_;
			std::map<std::string, SessionsManager::ptr> group_;
			std::mutex sessionsLocker_;
			std::mutex groupLocker_;
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
				auto userDataIter = clientIter->second->userData_.find(static_cast<int>(daxia::string(key).Hash()));
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
