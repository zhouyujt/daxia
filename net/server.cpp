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

		bool Server::Run(short port, const ExtraParam* extraParam)
		{
			if (extraParam == nullptr)
			{
				static ExtraParam defaultParam;
				extraParam = &defaultParam;
			}

			bool result = false;

			switch (extraParam->protocol_)
			{
			case common::Protocol_TCP:
				result = router_.RunAsTCP(port);
				break;
			case common::Protocol_UDP:
				result = router_.RunAsUDP(port);
				break;
			case common::Protocol_Websocket:
				break;
			case common::Protocol_HTTP:
			{
				const ExtraParamHttp* param = dynamic_cast<const ExtraParamHttp*>(extraParam);
				result = router_.RunAsHTTP(port, param->root_);
				break;
			}
#ifdef DAXIA_NET_SUPPORT_HTTPS
			case common::Protocol_HTTPS:
			{
				const ExtraParamHttps* param = dynamic_cast<const ExtraParamHttps*>(extraParam);
				result = router_.RunAsHTTPS(port, param->root_, param->pubCert_, param->priKey_);
				break;
			}
#endif
			default:
				break;
			}

			return result;
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
