#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include "router.h"
#include "sessions_manager.h"
#include "common/parser.h"
#include "common/http_parser.h"
#include "controller.h"

namespace daxia
{
	namespace net
	{
		Router::Router()
			: heartbeatSchedulerId_(-1)
			, nextSessionId_(1)
		{
			
		}

		Router::~Router()
		{

		}

		void Router::RunAsTCP(short port, bool enableFps)
		{
			if (!parser_)
			{
				parser_ = std::shared_ptr<common::Parser>(new common::DefaultParser);
			}

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
			scheduler_.Run(enableFps);
		}

		void Router::RunAsUDP(short port, bool enableFps)
		{
		}

		void Router::RunAsWebsocket(short port, const std::string& path, bool enableFps)
		{
		}

		void Router::RunAsHTTP(short port, const daxia::string& root, bool enableFps)
		{
			parser_ = std::shared_ptr<common::Parser>(new common::HttpServerParser);
			httpRoot_ = root;

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
			scheduler_.SetNetDispatch(std::bind(&Router::dispatchHttpMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			scheduler_.Run(enableFps);
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

		void Router::Handle(const char* url, std::shared_ptr<HttpController> controller)
		{
			controller->InitMethods();
			httpControllers_[url] = controller;
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

		void Router::dispatchMessage(std::shared_ptr<Session> client, int msgID, const common::Buffer data)
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

		void Router::dispatchHttpMessage(std::shared_ptr<Session> client, int msgID, const common::Buffer data)
		{
			using daxia::net::common::BasicSession;

			if (data.Size() == 0) return;

			common::HttpParser::RequestHeader header;
			size_t headerLen = header.InitFromData(data, data.Size());

			if (headerLen == -1) return;
			
			client->SetUserData(SESSION_USERDATA_REQUEST_INDEX, header);
			client->SetUserData(SESSION_USERDATA_RESPONSE_INDEX, common::HttpParser::ResponseHeader());

			auto iter = httpControllers_.find(header.StartLine.Url.GetString());
			if (iter != httpControllers_.end())
			{
				static const common::HttpParser::Methods methodsHelp;
				static const daxia::string methodGetHelp = daxia::string(methodsHelp.Get.Tag("http")).MakeLower();
				static const daxia::string methodPostHelp = daxia::string(methodsHelp.Post.Tag("http")).MakeLower();
				static const daxia::string methodPutHelp = daxia::string(methodsHelp.Put.Tag("http")).MakeLower();
				static const daxia::string methodHeadHelp = daxia::string(methodsHelp.Head.Tag("http")).MakeLower();
				static const daxia::string methodDeleteHelp = daxia::string(methodsHelp.Delete.Tag("http")).MakeLower();
				static const daxia::string methodOptionsHelp = daxia::string(methodsHelp.Options.Tag("http")).MakeLower();
				static const daxia::string methodTraceHelp = daxia::string(methodsHelp.Trace.Tag("http")).MakeLower();
				static const daxia::string methodConnectHelp = daxia::string(methodsHelp.Connect.Tag("http")).MakeLower();

				iter->second->SetContext(client);

				if (msgID == static_cast<int>(methodGetHelp.Hash())) { if (iter->second->Get) iter->second->Get(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodPostHelp.Hash())){ if (iter->second->Post) iter->second->Post(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodPutHelp.Hash())){ if (iter->second->Put)  iter->second->Put(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodHeadHelp.Hash())){ if (iter->second->Head) iter->second->Head(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodDeleteHelp.Hash())){ if (iter->second->Delete)  iter->second->Delete(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodOptionsHelp.Hash())){ if (iter->second->Options) iter->second->Options(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodTraceHelp.Hash())){ if (iter->second->Trace) iter->second->Trace(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }
				else if (msgID == static_cast<int>(methodConnectHelp.Hash())){ if (iter->second->Trace) iter->second->Trace(client.get(), this, common::Buffer(data + headerLen, data.Size() - headerLen)); }

				iter->second->ResetContext();
			}
			else
			{
				if (header.StartLine.Url == "/")
				{
					daxia::string html;
					html += "<html>\r\n";
					html += "<body>\r\n";
					html += "hello world!<br/>\r\n";
					html += "powered by daxia<br/>\r\n";
					html += "<a href=\"https://github.com/zhouyujt/daxia\">https://github.com/zhouyujt/daxia</a>\r\n";
					html += "</body>\r\n";
					html += "</html>\r\n";
					client->WriteMessage(0,html);
				}
				else
				{
					common::HttpParser::ResponseHeader* response = client->GetUserData<common::HttpParser::ResponseHeader>(SESSION_USERDATA_RESPONSE_INDEX);
					std::function<void()> serve404 = [&]()
					{
						if (response)
						{
							response->StartLine.StatusCode = "404";
							client->WriteMessage(0, nullptr, 0);
						}
					};

					// 获取后缀名
					size_t pos = header.StartLine.Url.Find(".");
					if (pos == -1)
					{
						serve404();
						return;
					}

					daxia::string extension = header.StartLine.Url.Mid(pos + 1, -1);
					daxia::string type = MIME_HELPER().Find(extension);
					if (type.IsEmpty())
					{
						serve404();
						return;
					}

					response->ContentType = type;

					std::ifstream ifs;
					daxia::string filename = httpRoot_ + header.StartLine.Url;
					ifs.open(filename);
					if (ifs.is_open())
					{
						ifs.seekg(0, ifs.end);
						size_t len = static_cast<size_t>(ifs.tellg());
						ifs.seekg(0, ifs.beg);

						daxia::buffer buffer;
						ifs.read(buffer.GetBuffer(len), len);
						ifs.close();

						client->WriteMessage(0, buffer);
					}
					else
					{
						serve404();
					}
				}
			}
		}

		int Router::getCoreCount() const
		{
			int count = std::thread::hardware_concurrency();

			return count == 0 ? 1 : count;
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

				scheduler_.PushNetRequest(session, common::DefMsgID_Connect, common::Buffer());
			}
		}

		void Router::onMessage(const boost::system::error_code& err, long long sessionId, int msgId, common::Buffer msg)
		{
			if (err || msgId == common::DefMsgID_DisConnect)
			{
				scheduler_.PushNetRequest(GetSession(sessionId), common::DefMsgID_DisConnect, common::Buffer(), [&, sessionId]()
				{
					DeleteSession(sessionId);
				});
			}
			else
			{
				scheduler_.PushNetRequest(GetSession(sessionId), msgId, msg);
			}
		}
	}// namespace net
}// namespace daxia

