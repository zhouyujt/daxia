#include "sessions_manager.h"

namespace daxia
{
	namespace dxg
	{
		SessionsManager::SessionsManager(Scheduler& scheduler)
			: scheduler_(scheduler)
			, heartbeatSchedulerId_(-1)
			, nextSessionId_(0)
		{
		}

		Session::ptr SessionsManager::AddSession(common::BasicSession::socket_ptr sock, std::shared_ptr<common::Parser> parser, Session::handler onMessage)
		{
			lock_guard locker(sessionsLocker_);

			Session::ptr  session(new Session(sock, parser, onMessage, nextSessionId_++));

			sessions_[session->GetSessionID()] = session;

			return session;
		}

		void SessionsManager::DeleteSession(long long id)
		{
			lock_guard locker(sessionsLocker_);

			sessions_.erase(id);
		}

		Session::ptr SessionsManager::GetSession(long long id)
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

		void SessionsManager::CreateGroup(const std::string& key)
		{
			lock_guard locker(groupLocker_);

			auto iter = group_.find(key);
			if (iter == group_.end())
			{
				group_[key] = std::shared_ptr<SessionsManager>(new SessionsManager(scheduler_));
			}
		}

		std::shared_ptr<SessionsManager> SessionsManager::GetGroup(const std::string& key)
		{
			lock_guard locker(groupLocker_);

			std::shared_ptr<SessionsManager> mgr;

			auto iter = group_.find(key);
			if (iter != group_.end())
			{
				mgr = iter->second;
			}

			return mgr;
		}

		void SessionsManager::EnableCheckHeartbeat(unsigned long interval)
		{
			if (heartbeatSchedulerId_ != -1)
			{
				// ¹Ø±ÕÐÄÌø¼ì²â
				scheduler_.Unschedule(heartbeatSchedulerId_);
				heartbeatSchedulerId_ = -1;
			}

			if (interval != 0)
			{
				// Æô¶¯ÐÄÌø¼ì²â
				heartbeatSchedulerId_ = scheduler_.Schedule([&, interval]()
				{
					using namespace std::chrono;

					time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());

					lock_guard locker(sessionsLocker_);

					for (auto iter = sessions_.begin(); iter != sessions_.end(); ++iter)
					{
						if (iter->second->GetLastReadTime().time_since_epoch().count() == 0)
						{
							if ((now - iter->second->GetConnectTime()).count() >= interval)
							{
								iter->second->Close();
							}
						}
						else
						{
							if ((now - iter->second->GetLastReadTime()).count() >= interval)
							{
								iter->second->Close();
							}
						}

					}
				}, 2000);
			}
		}
	}// namespace dxg
}// namespace daxia
