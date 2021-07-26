/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file sessions_manager.hpp
 * \author 漓江里的大虾
 * \date 三月 2018
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
		// 会话管理类
		class SessionsManager
		{
		public:
			typedef std::lock_guard<std::mutex> lock_guard;
			typedef std::shared_ptr<SessionsManager> ptr;
		public:
			SessionsManager();
			~SessionsManager(){}
		public:
			// 增加一个客户端
			void AddSession(Session::ptr session);
			// 删除一个客户端
			void DeleteSession(long long id);
			// 删除所有客户端
			void DeleteAllSession();
			// 获取客户端
			Session::ptr GetSession(long long id);
			// 根据自定义数据获取客户端
			template<class T>
			Session::ptr GetSession(const std::string& key, const T& data);
			// 向一个客户端组广播一条消息
			void Broadcast(const std::string& name,const std::string& msg);
			// 创建一个客户端组
			SessionsManager::ptr CreateGroup(const std::string& name);
			// 删除一个客户端组
			void DeleteGroup(const std::string& name);
			// 删除所有客户端组
			void DeleteAllGroup();
			// 获取客户端组
			SessionsManager::ptr GetGroup(const std::string& name);
			// 枚举客户端
			void EnumSession(std::function<bool(Session::ptr)> func);
			// 枚举客户端组
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
