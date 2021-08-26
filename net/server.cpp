#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "server.h"

namespace daxia
{
	namespace net
	{
		Server::Server()
		{

		}

		Server::~Server()
		{
			Stop();
		}
		void Server::Run(short port, common::Protocol protcol, bool enableFps, const char* root)
		{
			switch (protcol)
			{
			case common::Protocol_TCP:
				router_.RunAsTCP(port, enableFps);
				break;
			case common::Protocol_UDP:
				router_.RunAsUDP(port, enableFps);
				break;
			case common::Protocol_Websocket:
				router_.RunAsWebsocket(port, root, enableFps);
				break;
			case common::Protocol_HTTP:
				router_.RunAsHTTP(port, root, enableFps);
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

		void Server::Handle(const char* url, std::shared_ptr<HttpController> controller)
		{
			router_.Handle(url, controller);
		}

		void Server::EnableCheckHeartbeat(unsigned long milliseconds)
		{
			router_.EnableCheckHeartbeat(milliseconds);
		}

		void Server::SetFPS(unsigned long fps)
		{
			router_.GetScheduler().SetFps(fps);
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

		daxia::net::SessionsManager& Server::GetSessionManager()
		{
			return router_;
		}

	}// namespace net
}// namespace daxia
