#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "server.h"

namespace daxia
{
	namespace dxg
	{
		Server::Server()
		{

		}

		Server::~Server()
		{
			Stop();
		}
		void Server::Run(short port, common::Protocol protcol)
		{
			switch (protcol)
			{
			case common::Protocol_TCP:
				router_.RunAsTCP(port);
				break;
			case common::Protocol_UDP:
				router_.RunAsUDP(port);
				break;
			case common::Protocol_Websocket:
				router_.RunAsWebsocket(port, websocketPath_);
				break;
			case common::Protocol_HTTP:
				router_.RunAsHTTP(port);
				break;
			default:
				break;
			}
		}

		void Server::Stop()
		{
			router_.Stop();
		}

		void Server::SetParser(std::shared_ptr<common::Parser> parser)
		{
			router_.SetParser(parser);
		}

		void Server::Handle(int msgId, std::shared_ptr<Controller> controller)
		{
			router_.Handle(msgId, controller);
		}

		void Server::EnableCheckHeartbeat(unsigned long milliseconds)
		{
			router_.EnableCheckHeartbeat(milliseconds);
		}

		void Server::SetFPS(unsigned long fps)
		{
			router_.GetScheduler().SetFPS(fps);
		}

		long long Server::ScheduleUpdate(Scheduler::scheduleFunc func)
		{
			return router_.GetScheduler().ScheduleUpdate(func);
		}

		long long Server::Schedule(Scheduler::scheduleFunc func, unsigned long duration)
		{
			return router_.GetScheduler().Schedule(func, duration);
		}

		long long Server::ScheduleOnce(Scheduler::scheduleFunc func, unsigned long duration)
		{
			return router_.GetScheduler().ScheduleOnce(func, duration);
		}

		void Server::UnscheduleUpdate(long long scheduleID)
		{
			router_.GetScheduler().UnscheduleUpdate(scheduleID);
		}

		void Server::Unschedule(long long scheduleID)
		{
			router_.GetScheduler().Unschedule(scheduleID);
		}

		void Server::UnscheduleAll()
		{
			router_.GetScheduler().UnscheduleAll();
		}

		const daxia::dxg::SessionsManager& Server::GetSessionManager() const
		{
			return router_;
		}

	}// namespace dxg
}// namespace daxia
