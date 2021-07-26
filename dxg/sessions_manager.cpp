#include "sessions_manager.h"

namespace daxia
{
	namespace dxg
	{
		SessionsManager::SessionsManager()
		{
		}

		void SessionsManager::AddSession(Session::ptr session)
		{
			lock_guard locker(sessionsLocker_);

			sessions_[session->GetSessionID()] = session;
		}

		void SessionsManager::DeleteSession(long long id)
		{
			lock_guard locker(sessionsLocker_);

			sessions_.erase(id);
		}

		void SessionsManager::DeleteAllSession()
		{
			lock_guard locker(sessionsLocker_);

			sessions_.clear();
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

		void SessionsManager::Broadcast(const std::string& name, const std::string& msg)
		{
			auto group = GetGroup(name);
			if (group)
			{
				group->EnumSession([&](Session::ptr session)
				{
					session->WriteMessage(msg);
					return true;
				});
			}
		}

		SessionsManager::ptr SessionsManager::CreateGroup(const std::string& name)
		{
			lock_guard locker(groupLocker_);

			auto iter = group_.find(name);
			if (iter == group_.end())
			{
				group_[name] = std::shared_ptr<SessionsManager>(new SessionsManager);
			}

			return GetGroup(name);
		}

		void SessionsManager::DeleteGroup(const std::string& name)
		{
			lock_guard locker(groupLocker_);

			group_.erase(name);

		}

		void SessionsManager::DeleteAllGroup()
		{
			lock_guard locker(groupLocker_);

			group_.clear();
		}

		SessionsManager::ptr SessionsManager::GetGroup(const std::string& name)
		{
			lock_guard locker(groupLocker_);

			std::shared_ptr<SessionsManager> mgr;

			auto iter = group_.find(name);
			if (iter != group_.end())
			{
				mgr = iter->second;
			}

			return mgr;
		}

		void SessionsManager::EnumSession(std::function<bool(Session::ptr)> func)
		{
			lock_guard locker(sessionsLocker_);

			for (const std::pair<long long, Session::ptr>& session : sessions_)
			{
				if (!func(session.second))
				{
					break;
				}
			}
		}

		void SessionsManager::EnumGroup(std::function<bool(SessionsManager::ptr)> func)
		{
			lock_guard locker(groupLocker_);

			for (const std::pair<std::string, SessionsManager::ptr>& group : group_)
			{
				if (!func(group.second))
				{
					break;
				}
			}
		}
	}// namespace dxg
}// namespace daxia
