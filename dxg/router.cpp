#ifdef _MSC_VER
#include <sdkddkver.h>
#endif
#include "router.h"
#include "sessions_manager.h"
#include "common/parser.h"
#include "controller.h"

namespace daxia
{
	namespace dxg
	{
		Router::Router()
			: heartbeatSchedulerId_(-1)
			, nextSessionId_(0)
		{
			parser_ = std::shared_ptr<common::Parser>(new common::DefaultParser);
		}

		Router::~Router()
		{

		}

		void Router::RunAsTCP(short port)
		{
			endpoint ep(boost::asio::ip::tcp::v4(), port);
			acceptor_ = acceptor_ptr(new acceptor(ios_, ep));

			socket_ptr socketSession(new socket(ios_));
			acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

			// 启动I/O线程
			int coreCount = getCoreCount();
			for (int i = 0; i < coreCount * 2; ++i)
			{
				ioThreads_.push_back(
					std::thread([=]()
				{
					ios_.run();
				}));
			}

			// 启动调度器
			scheduler_.SetNetDispatch(std::bind(&Router::dispatchMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			scheduler_.Run();
		}

		void Router::RunAsUDP(short port)
		{
		}

		void Router::RunAsWebsocket(short port, const std::string& path)
		{
		}

		void Router::RunAsHTTP(short port)
		{

		}

		void Router::SetParser(std::shared_ptr<common::Parser> parser)
		{
			parser_ = parser;
		}

		void Router::Stop()
		{
			// 结束I/O线程
			ios_.stop();
			for (size_t i = 0; i < ioThreads_.size(); ++i)
			{
				if (ioThreads_[i].joinable())
				{
					ioThreads_[i].join();
				}
			}
			ios_.reset();

			// 结束调度器
			scheduler_.Stop();
		}

		void Router::Handle(int msgID, std::shared_ptr<Controller> controller)
		{
			controllers_[msgID] = controller;
		}

		void Router::EnableCheckHeartbeat(unsigned long interval)
		{
			if (heartbeatSchedulerId_ != -1)
			{
				// 关闭心跳检测
				scheduler_.Unschedule(heartbeatSchedulerId_);
				heartbeatSchedulerId_ = -1;
			}

			if (interval != 0)
			{
				// 启动心跳检测
				heartbeatSchedulerId_ = scheduler_.Schedule([&, interval]()
				{
					using namespace std::chrono;

					time_point<system_clock, milliseconds> now = time_point_cast<milliseconds>(system_clock::now());

					EnumSession([&](Session::ptr session)
					{
						if (session->GetLastReadTime().time_since_epoch().count() == 0)
						{
							if ((now - session->GetConnectTime()).count() >= interval)
							{
								session->Close();
							}
						}
						else
						{
							if ((now - session->GetLastReadTime()).count() >= interval)
							{
								session->Close();
							}
						}

						return true;
					});
				}, 2000);
			}
		}

		Scheduler& Router::GetScheduler()
		{
			return scheduler_;
		}

		void Router::dispatchMessage(std::shared_ptr<Session> client, int msgID, const common::shared_buffer data)
		{
			auto iter = controllers_.find(msgID);
			if (iter != controllers_.end())
			{
				iter->second->Proc(msgID, client.get(), this, data);
			}
			else
			{
				auto iter = controllers_.find(common::DefMsgID_UnHandle);
				if (iter != controllers_.end())
				{
					iter->second->Proc(msgID, client.get(), this, data);
				}
			}
		}

		int Router::getCoreCount() const
		{
			int count = 1; // 至少一个

#if !defined (_MSC_VER)
			count = sysconf(_SC_NPROCESSORS_CONF);
#else
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			count = si.dwNumberOfProcessors;
#endif  

			return count;
		}

		void Router::onAccept(socket_ptr sock, const error_code& err)
		{
			if (!err)
			{
				socket_ptr socketSession(new socket(ios_));
				acceptor_->async_accept(*socketSession, bind(&Router::onAccept, this, socketSession, std::placeholders::_1));

				sessionIdLocker_.lock();
				Session::ptr  session(new Session(sock, parser_, std::bind(&Router::onMessage,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4), nextSessionId_++));
				sessionIdLocker_.unlock();

				session->UpdateConnectTime();
				AddSession(session);

				scheduler_.PushNetRequest(session, common::DefMsgID_Connect, common::shared_buffer());
			}
		}

		void Router::onMessage(const boost::system::error_code& err, long long sessionId, int msgId, common::shared_buffer msg)
		{
			if (err || msgId == common::DefMsgID_DisConnect)
			{
				scheduler_.PushNetRequest(GetSession(sessionId), common::DefMsgID_DisConnect, common::shared_buffer(), [&, sessionId]()
				{
					DeleteSession(sessionId);
				});
			}
			else
			{
				scheduler_.PushNetRequest(GetSession(sessionId), msgId, msg);
			}
		}
	}// namespace dxg
}// namespace daxia

