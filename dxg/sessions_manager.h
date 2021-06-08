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

#ifndef __DAXIA_DXG_SERVER_SESSIONS_MANAGER_HPP
#define __DAXIA_DXG_SERVER_SESSIONS_MANAGER_HPP

#include <daxia/dxg/server/session.hpp>
#include <daxia/dxg/server/scheduler.hpp>

namespace daxia
{
	namespace dxg
	{
		namespace server
		{
			// 会话管理类
			class SessionsManager
			{
			public:
				typedef std::shared_ptr<SessionsManager> ptr;
				typedef std::lock_guard<std::mutex> lock_guard;
			public:
				SessionsManager(Scheduler& scheduler);
				~SessionsManager(){}
			public:
				// 增加一个客户端
				Session::ptr AddSession(common::BasicSession::socket_ptr sock, common::Parser::ptr parser, Session::handler onMessage);

				// 删除一个客户端
				void DeleteSession(long long id);

				// 获取客户端
				Session::ptr GetSession(long long id);

				// 根据自定义数据获取客户端
				template<class T>
				Session::ptr GetSession(const std::string& key, const T& data);

				// 广播一条消息
				void Broadcast();

				// 创建一个客户端组
				void CreateGroup(const std::string& key);

				// 获取客户端组
				ptr GetGroup(const std::string& key);

				// 开启/关闭心跳检测
				void EnableCheckHeartbeat(unsigned long interval);
			private:
				std::map<long long, Session::ptr> sessions_;
				std::map<std::string, ptr> group_;
				std::mutex sessionsLocker_;
				std::mutex groupLocker_;
				Scheduler& scheduler_;
				long long heartbeatSchedulerId_;
				long long nextSessionId_;
			};

			//////////////////////////////////////////////////////////////////////////
			inline SessionsManager::SessionsManager(Scheduler& scheduler)
				: scheduler_(scheduler)
				, heartbeatSchedulerId_(-1)
				, nextSessionId_(0)
			{
			}

			inline Session::ptr SessionsManager::AddSession(common::BasicSession::socket_ptr sock, common::Parser::ptr parser, Session::handler onMessage)
			{
				lock_guard locker(sessionsLocker_);

				Session::ptr  session(new Session(sock, parser, onMessage, nextSessionId_++));

				sessions_[session->GetSessionID()] = session;

				return session;
			}

			inline void SessionsManager::DeleteSession(long long id)
			{
				lock_guard locker(sessionsLocker_);

				sessions_.erase(id);
			}

			inline Session::ptr SessionsManager::GetSession(long long id)
			{
				Session::ptr session;

				lock_guard locker(sessionsLocker_);

				auto iter = sessions_.find(id);
				if (iter != sessions_.end())
				{
					session = iter->second;
				}

				return session;
			}

			template<class T>
			inline Session::ptr SessionsManager::GetSession(const std::string& key, const T& data)
			{
				client_ptr client = nullptr;

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
							if (boost::any_cast<T>(userDataIter->second) == data) client = clientIter->second;
						}
						catch (...)
						{
						}

						if (client) break;
					}
				}

				return client;
			}

			inline void SessionsManager::CreateGroup(const std::string& key)
			{
				lock_guard locker(groupLocker_);

				auto iter = group_.find(key);
				if (iter == group_.end())
				{
					group_[key] = ptr(new SessionsManager(scheduler_));
				}
			}

			inline SessionsManager::ptr SessionsManager::GetGroup(const std::string& key)
			{
				lock_guard locker(groupLocker_);

				ptr mgr;

				auto iter = group_.find(key);
				if (iter != group_.end())
				{
					mgr = iter->second;
				}

				return mgr;
			}

			inline void SessionsManager::EnableCheckHeartbeat(unsigned long interval)
			{
				if (interval == 0 && heartbeatSchedulerId_ != -1)
				{
					// 关闭心跳检测
					scheduler_.Unschedule(heartbeatSchedulerId_);
				}
				else
				{
					// 启动心跳检测
					heartbeatSchedulerId_ = scheduler_.Schedule([&,interval]()
					{
						using namespace std::chrono;

						time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());

						lock_guard locker(sessionsLocker_);

						for (auto iter = sessions_.begin(); iter != sessions_.end(); ++iter)
						{
							if ((now - iter->second->GetLastReadTime()).count() >= interval)
							{
								iter->second->Close();
							}
						}
					}, 2000);
				}
			}

		}// namespace server
	}// namespace dxg
}// namespace daxia

#endif // !__DAXIA_DXG_SERVER_SESSIONS_MANAGER_HPP
